#include "worker.h"

#include "common.h"
#include "log.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define LOG_TYPE LT_WORKER

struct job;

struct worker {
	struct job*     w_job;
	pthread_cond_t  w_cv;
	pthread_mutex_t w_mutex;
};

struct worker* worker_create() {
	struct worker* w = NULL;
	int result;

	w = malloc(sizeof(struct worker));
	w->w_job = NULL;

	CHECK_LOCK_INIT(w->w_mutex);
	CHECK_COND_INIT(w->w_cv);

	return w;
failure:
	worker_destroy(w);
	return NULL;
}

void worker_destroy(struct worker* w) {
	if (!w) {
		LOG(LL_WARN, "null worker");
		return;
	}

	if (w->w_job) {
		LOG(LL_WARN, "non-null job");
	}

	pthread_mutex_destroy(&w->w_mutex);
	pthread_cond_destroy(&w->w_cv);

	free(w);
}
