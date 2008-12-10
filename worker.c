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
				w->w_job == NULL) {
			pthread_cond_wait(&w->w_cv, &w->w_mutex);
		}

		if (w->w_shutdown && !w->w_job) {
			break;
		}

		CHECK_LOGE(job_run(w->w_job));

		if (w->w_job->j_state != JS_COMPLETE) {
			w->w_job->j_state = JS_ERROR;
		}

		CHECK_LOGE(dispatcher_notify(w->w_dispatcher, w, w->w_job));
		w->w_job = NULL;

		if (result != 0) {
			w->w_state = WS_ERROR;
			CHECK_UNLOCK(w->w_mutex);
			return result;
		}

		w->w_state = WS_WAITING;
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
	int result;

	if (!w) {
		LOG(LL_WARN, "null worker");
		return;
	}

	if (w->w_job) {
		LOG(LL_WARN, "non-null job");
	}

	if (w->w_state != WS_SHUTDOWN && w->w_state != WS_ERROR) {
		LOG(LL_WARN, "not in WS_SHUTDOWN or WS_ERROR");
	}

	CHECK_LOCK_DEST(w->w_mutex);
	CHECK_COND_DEST(w->w_cv);

	free(w);
}

/*****************************************************************************/
int worker_shutdown(struct worker* w) {
	PRE(w != NULL);

	CHECK_LOCK(w->w_mutex);
	w->w_shutdown = 1;
	pthread_cond_broadcast(&w->w_cv);
	CHECK_UNLOCK(w->w_mutex);

	return 0;
}

/*****************************************************************************/
int worker_assign(struct worker* w, struct job* j) {
	int retval = 0;

	PRE(w != NULL);
	PRE(j != NULL);

	CHECK_LOCK(w->w_mutex);

	PREG(w->w_job == NULL);
	PREG(w->w_state == WS_WAITING);

	w->w_job = j;
	pthread_cond_broadcast(&w->w_cv);

failure:
	CHECK_UNLOCK(w->w_mutex);
	return retval;
}

/*****************************************************************************/
enum worker_state worker_state(struct worker* w) {
	return w->w_state;
}

/*****************************************************************************/
struct job* worker_job(struct worker* w) {
	return w->w_job;
}
