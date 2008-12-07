#include "batch.h"

#include "check.h"
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
	unsigned int    b_num_jobs;
	unsigned int    b_job_idx;
};

/*****************************************************************************/
struct batch* batch_create(const char* name) {
	struct batch* b = calloc(1, sizeof(struct batch));

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
	
	CHECK_LOCK_INIT(b->b_mutex);
	CHECK_COND_INIT(b->b_job_cv);

	return b;

failure:
	batch_destroy(b);
	return NULL;
}

/*****************************************************************************/
void batch_destroy(struct batch* b) {
	int result;

	if (!b) {
		LOG(LL_WARN, "tried to destroy null batch");
		return;
	}

	LOG(LL_DEBUG, "destroying batch '%s'", b->b_name);

	if (b->b_name) {
		free(b->b_name);
	}

	if (b->b_queue) {
		if ((result = queue_destroy(b->b_queue))) {
			LOG(LL_WARN, "queue_destroy: %s", strerror(result));
		}
	}

	CHECK_LOCK_DEST(b->b_mutex);
	CHECK_COND_DEST(b->b_job_cv);

	free(b);
}

/*****************************************************************************/
const char* batch_name(const struct batch* b) {
	return b->b_name;
}

/*****************************************************************************/
int batch_add_job(struct batch* b, struct job* j) {
	int retval = 0;

	PRE(b != NULL);
	PRE(j != NULL);
	PRE(j->j_id == 0);

	LOG(LL_DEBUG, "adding a job to batch %s", b->b_name);

	CHECK_LOCK(b->b_mutex);

	CHECKF(queue_push(b->b_queue, j));

	b->b_num_jobs++;
	j->j_id = b->b_job_idx++;
	LOG(LL_DEBUG, "added job with id %d", j->j_id);

	pthread_cond_broadcast(&b->b_job_cv);

failure:
	CHECK_UNLOCK(b->b_mutex);

	return retval;
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
	
	PREN(b != NULL);

	/* must use CHECKN because we return NULL instead of the result */
	CHECKN(pthread_mutex_lock(&b->b_mutex));

	while (b->b_num_jobs == 0) {
		LOG(LL_DEBUG, "no jobs, waiting");
		pthread_cond_wait(&b->b_job_cv, &b->b_mutex);
	}

	CHECK_LOGE(queue_pop(b->b_queue, (void**)&ret));

	if (result != 0) {
		ret = NULL;
	} else {
		b->b_num_jobs--;
		LOG(LL_DEBUG, "got job %d, %d remaining", ret->j_id, b->b_num_jobs);
	}

	CHECKN(pthread_mutex_unlock(&b->b_mutex));

	return ret;
}
