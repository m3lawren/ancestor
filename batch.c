#include "batch.h"

#include "job.h"
#include "log.h"

#include <assert.h>
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
		LOG(LL_ERROR, "unable to create b_mutex: %s", strerror(result));
		goto failure;
	}

	if ((result = pthread_cond_init(&b->b_job_cv, NULL))) {
		LOG(LL_ERROR, "unable to create b_job_cv: %s", strerror(result));
		goto failure;
	}

	batch_incref(b);

	return b;

failure:
	if (b->b_name) {
		free(b->b_name);
	}
	if (b->b_queue) {
		assert(0 == queue_destroy(b->b_queue));
	}
	pthread_mutex_destroy(&b->b_mutex);
	pthread_cond_destroy(&b->b_job_cv);
	free(b);
	return NULL;
}

/*****************************************************************************/
static void _batch_destroy(struct batch* b) {
	if (!b) {
		return;
	}

	LOG(LL_DEBUG, "destroying batch %s", b->b_name);
	
	if (b->b_name) {
		free(b->b_name);
	}

	if (b->b_queue) {
		assert(0 == queue_destroy(b->b_queue));
	}

	pthread_mutex_destroy(&b->b_mutex);
	pthread_cond_destroy(&b->b_job_cv);

	free(b);
}

/*****************************************************************************/
void batch_incref(struct batch* b) {
	assert(b);

	LOG(LL_DEBUG, "%p", b);

	assert(0 == pthread_mutex_lock(&b->b_mutex));
	b->b_num_refs++;
	assert(0 == pthread_mutex_unlock(&b->b_mutex));
}

/*****************************************************************************/
void batch_decref(struct batch* b) {
	int can_delete = 0;

	assert(b);

	LOG(LL_DEBUG, "%p", b);

	assert(0 == pthread_mutex_lock(&b->b_mutex));
	b->b_num_refs--;
	if (!b->b_num_refs) {
		can_delete = 1;
	}
	assert(0 == pthread_mutex_unlock(&b->b_mutex));

	if (can_delete) {
		_batch_destroy(b);
	}
}

/*****************************************************************************/
const char* batch_name(const struct batch* b) {
	return b->b_name;
}

/*****************************************************************************/
int batch_add_job(struct batch* b, struct job* j) {
	int ret;

	assert(b);
	assert(j);
	assert(j->j_id == 0);

	LOG(LL_DEBUG, "adding a job to batch %s", b->b_name);

	assert(0 == pthread_mutex_lock(&b->b_mutex));
	if (!(ret = queue_push(b->b_queue, j))) {
		b->b_num_jobs++;
		j->j_id = b->b_job_idx++;
		LOG(LL_DEBUG, "added job with id %d", j->j_id);
	} else {
		LOG(LL_ERROR, "unable to add job: %s", strerror(ret));
	}
	pthread_cond_broadcast(&b->b_job_cv);
	assert(0 == pthread_mutex_unlock(&b->b_mutex));

	return ret;
}

/*****************************************************************************/
unsigned int batch_num_jobs(const struct batch* b) {
	assert(b);
	return b->b_num_jobs;
}

/*****************************************************************************/
struct job* batch_next_job(struct batch* b) {
	struct job* ret;

	assert(b);

	assert(0 == pthread_mutex_lock(&b->b_mutex));
	while (b->b_num_jobs == 0) {
		LOG(LL_DEBUG, "no jobs, waiting");
		pthread_cond_wait(&b->b_job_cv, &b->b_mutex);
	}
	assert(0 == queue_pop(b->b_queue, (void**)&ret));
	b->b_num_jobs--;
	LOG(LL_DEBUG, "got job %d, %d remaining", ret->j_id, b->b_num_jobs);
	assert(0 == pthread_mutex_unlock(&b->b_mutex));

	return ret;
}
