#include "batch.h"

#include "common.h"
#include "job.h"
#include "log.h"

#include <errno.h>
#include <pthread.h>
#include <queue.h>
#include <stdlib.h>
#include <string.h>

#define LOG_TYPE LT_BATCH

struct batch {
	pthread_mutex_t b_mutex;
	pthread_cond_t  b_job_cv;
	char*           b_name;
	struct queue*   b_queue;
	unsigned int    b_num_refs;
	unsigned int    b_num_jobs;
	unsigned int    b_job_idx;
};

/*****************************************************************************/
static void _batch_destroy(struct batch* b) {
	int result;

	if (!b) {
		LOG(LL_WARN, "tried to destroy null batch");
		return;
	}

	LOG(LL_DEBUG, "destroying batch '%s'", b->b_name);

	if (b->b_num_refs > 0) {
		LOG(LL_WARN, "'%s' has %d refs", b->b_num_refs);
	}
	
	if (b->b_name) {
		free(b->b_name);
	}

	if (b->b_queue) {
		if ((result = queue_destroy(b->b_queue))) {
			LOG(LL_WARN, "queue_destroy: %s", strerror(result));
		}
	}

	pthread_mutex_destroy(&b->b_mutex);
	pthread_cond_destroy(&b->b_job_cv);

	free(b);
}

/*****************************************************************************/
struct batch* batch_create(const char* name) {
	struct batch* b = calloc(1, sizeof(struct batch));
	int result = 0;

	LOG(LL_DEBUG, "creating new batch '%s'", name);

	if (!b) {
		LOG(LL_ERROR, "unable to allocate space for struct batch");
		return NULL;
	}

	if (!(b->b_name = malloc(strlen(name) + 1))) {
		LOG(LL_ERROR, "unable to allocate space for b_name");
		goto failure;
	}

	strcpy(b->b_name, name);

	if (!(b->b_queue = queue_create())) {
		LOG(LL_ERROR, "unable to create b_queue");
		goto failure;
	}

	if ((result = pthread_mutex_init(&b->b_mutex, NULL))) {
		LOG(LL_ERROR, "pthread_mutex_init: %s", strerror(result));
		goto failure;
	}

	if ((result = pthread_cond_init(&b->b_job_cv, NULL))) {
		LOG(LL_ERROR, "pthread_cond_init: %s", strerror(result));
		goto failure;
	}

	if ((result = batch_incref(b))) {
		LOG(LL_ERROR, "batch_incref: %s", strerror(result));
		goto failure;
	}

	return b;

failure:
	_batch_destroy(b);
	return NULL;
}

/*****************************************************************************/
int batch_incref(struct batch* b) {
	int result;

	if (!b) {
		LOG(LL_ERROR, "null batch");
		return EINVAL;
	}

	CHECK_LOCK(b->b_mutex);

	b->b_num_refs++;
	LOG(LL_DEBUG, "'%s' has %d refs", b->b_name, b->b_num_refs);

	CHECK_UNLOCK(b->b_mutex);

	return 0;
}

/*****************************************************************************/
int batch_decref(struct batch* b) {
	int can_delete = 0;
	int result;

	if (!b) {
		LOG(LL_ERROR, "null batch");
		return EINVAL;
	}

	CHECK_LOCK(b->b_mutex);

	b->b_num_refs--;
	LOG(LL_DEBUG, "'%s' has %d refs", b->b_name, b->b_num_refs);
	if (!b->b_num_refs) {
		can_delete = 1;
	}

	CHECK_UNLOCK(b->b_mutex);

	if (can_delete) {
		_batch_destroy(b);
	}

	return 0;
}

/*****************************************************************************/
const char* batch_name(const struct batch* b) {
	return b->b_name;
}

/*****************************************************************************/
int batch_add_job(struct batch* b, struct job* j) {
	int ret;
	int result;

	CHECK_NULL(b);
	CHECK_NULL(j);
	CHECK_COND(j->j_id != 0);

	LOG(LL_DEBUG, "adding a job to batch %s", b->b_name);

	CHECK_LOCK(b->b_mutex);

	if (!(ret = queue_push(b->b_queue, j))) {
		b->b_num_jobs++;
		j->j_id = b->b_job_idx++;
		LOG(LL_DEBUG, "added job with id %d", j->j_id);
	} else {
		LOG(LL_ERROR, "unable to add job: %s", strerror(ret));
	}
	pthread_cond_broadcast(&b->b_job_cv);

	CHECK_UNLOCK(b->b_mutex);

	return ret;
}

/*****************************************************************************/
unsigned int batch_num_jobs(const struct batch* b) {
	if (!b) {
		return 0;
	}
	return b->b_num_jobs;
}

/*****************************************************************************/
struct job* batch_next_job(struct batch* b) {
	struct job* ret;
	int result;

	if (!b) {
		LOG(LL_ERROR, "null batch");
		return NULL;
	}

	CHECK_LOCK_GOTO(b->b_mutex);

	while (b->b_num_jobs == 0) {
		LOG(LL_DEBUG, "no jobs, waiting");
		pthread_cond_wait(&b->b_job_cv, &b->b_mutex);
	}
	if ((result = queue_pop(b->b_queue, (void**)&ret))) {
		LOG(LL_ERROR, "queue_pop: %s", strerror(result));
		ret = NULL;
	} else {
		b->b_num_jobs--;
		LOG(LL_DEBUG, "got job %d, %d remaining", ret->j_id, b->b_num_jobs);
	}

	CHECK_UNLOCK_GOTO(b->b_mutex);

	return ret;
failure:
	return NULL;
}
