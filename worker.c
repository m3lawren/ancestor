#include "worker.h"

#include "check.h"
#include "log.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define LOG_TYPE LT_WORKER

struct job;

struct worker {
	struct job*       w_job;
	pthread_cond_t    w_cv;
	pthread_mutex_t   w_mutex;
	pthread_t         w_thread;
	enum worker_state w_state;
	int               w_shutdown;
};

/******************************************************************************/
int worker_impl(struct worker* w) {
	int result;

	LOG(LL_DEBUG, "worker started");

	CHECK_LOCK(w->w_mutex);
	while (w->w_shutdown == 0) {
		pthread_cond_wait(&w->w_cv, &w->w_mutex);
	}
	CHECK_UNLOCK(w->w_mutex);

	return 0;
}

/******************************************************************************/
static void* worker_thread(void* arg) {
	worker_impl(arg);
	return NULL;
}

/******************************************************************************/
struct worker* worker_create() {
	struct worker* w = NULL;
	int result;

	w = malloc(sizeof(struct worker));
	w->w_job = NULL;
	w->w_state = WS_WAITING;
	w->w_shutdown = 0;

	CHECK_LOCK_INIT(w->w_mutex);
	CHECK_COND_INIT(w->w_cv);

	pthread_create(&w->w_thread, NULL, worker_thread, w);

	return w;
failure:
	worker_destroy(w);
	return NULL;
}

/******************************************************************************/
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

/*****************************************************************************/
enum worker_state worker_state(struct worker* w) {
	return w->w_state;
}

/*****************************************************************************/
int worker_shutdown(struct worker* w) {
	int result;

	PRE(w != NULL);

	CHECK_LOCK(w->w_mutex);
	w->w_shutdown = 1;
	pthread_cond_broadcast(&w->w_cv);
	CHECK_UNLOCK(w->w_mutex);

	return 0;
}
