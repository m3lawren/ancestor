#include "worker.h"

#include "check.h"
#include "dispatcher.h"
#include "job.h"
#include "log.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define LOG_TYPE LT_WORKER

struct job;

struct worker {
	struct job*        w_job;
	struct dispatcher* w_dispatcher;
	pthread_cond_t     w_cv;
	pthread_mutex_t    w_mutex;
	pthread_t          w_thread;
	enum worker_state  w_state;
	int                w_shutdown;
};

/******************************************************************************/
int worker_impl(struct worker* w) {
	int result;

	PRE(w->w_thread == pthread_self());

	LOG(LL_DEBUG, "worker started");

	CHECK_LOCK(w->w_mutex);
	while (1) {
		while (w->w_shutdown == 0 && 
				(w->w_job == NULL || 
				 w->w_job->j_state == JS_COMPLETE ||
				 w->w_job->j_state == JS_ERROR)) {
			pthread_cond_wait(&w->w_cv, &w->w_mutex);
		}

		if (w->w_shutdown) {
			break;
		}

		CHECK_CALL(job_run(w->w_job)) { 
			LOG_CALL_WARN(job_run);
		}

		if (w->w_job->j_state != JS_COMPLETE) {
			w->w_job->j_state = JS_ERROR;
		}

		CHECK_CALL(dispatcher_notify(w->w_dispatcher, w)) {
			LOG_CALL_ERROR(dispatcher_notify);
			w->w_state = WS_ERROR;
			CHECK_UNLOCK(w->w_mutex);
			return result;
		}
	}
	w->w_state = WS_SHUTDOWN;
	CHECK_UNLOCK(w->w_mutex);

	return 0;
}

/******************************************************************************/
static void* worker_thread(void* arg) {
	worker_impl(arg);
	return NULL;
}

/******************************************************************************/
struct worker* worker_create(struct dispatcher* d) {
	struct worker* w = NULL;
	int result;

	w = malloc(sizeof(struct worker));
	w->w_job = NULL;
	w->w_state = WS_WAITING;
	w->w_shutdown = 0;
	w->w_dispatcher = d;

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
int worker_shutdown(struct worker* w) {
	int result;

	PRE(w != NULL);

	CHECK_LOCK(w->w_mutex);
	w->w_shutdown = 1;
	pthread_cond_broadcast(&w->w_cv);
	CHECK_UNLOCK(w->w_mutex);

	return 0;
}


/*****************************************************************************/
enum worker_state worker_state(struct worker* w) {
	return w->w_state;
}

/*****************************************************************************/
struct job* worker_job(struct worker* w) {
	return w->w_job;
}
