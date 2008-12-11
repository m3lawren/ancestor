#include "dispatcher.h"

#include "batch.h"
#include "check.h"
#include "job.h"
#include "log.h"
#include "worker.h"

#include <array.h>
#include <errno.h>
#include <pthread.h>
#include <queue.h>
#include <stdlib.h>
#include <string.h>

#define LOG_TYPE LT_DISPATCHER

struct dispatcher {
	char*           d_name;
	
	pthread_cond_t  d_cv;
	pthread_mutex_t d_mutex;

	struct array*   d_batches;

	struct worker** d_workers;
	unsigned int    d_num_workers;
	struct queue*   d_free_workers;
	unsigned int    d_num_free_workers;
};

/*****************************************************************************/
struct dispatcher* dispatcher_create(const char* name) {
	struct dispatcher* d;

	if (!(d = malloc(sizeof(struct dispatcher)))) {
		LOG(LL_ERROR, "unable to allocate space for struct");
		goto failure;
	}

	d->d_name = NULL;
	d->d_batches = NULL;
	d->d_num_workers = DISPATCHER_WORKERS;
	d->d_num_free_workers = 0;
	d->d_workers = NULL;

	if (!(d->d_name = malloc(strlen(name) + 1))) {
		LOG(LL_ERROR, "unable to allocate space for name");
		goto failure;
	}

	strcpy(d->d_name, name);

	if (!(d->d_batches = array_create())) {
		LOG(LL_ERROR, "unable to allocate space for batches");
		goto failure;
	}

	if (!(d->d_free_workers = queue_create())) {
		LOG(LL_ERROR, "unable to allocate free workers list");
		goto failure;
	}

	CHECK_LOCK_INIT(d->d_mutex);
	CHECK_COND_INIT(d->d_cv);

	return d;
failure:
	dispatcher_destroy(d);
	return NULL;
}

/*****************************************************************************/
int dispatcher_destroy(struct dispatcher* d) {
	int result;
	unsigned int i;

	PRE(d != NULL);

	/* TODO: add proper shutdown logic to ensure all workers are stopped */
	PRE(d->d_num_free_workers == d->d_num_workers);

	if (d->d_name) {
		free(d->d_name);
	}
	if (d->d_batches) {
		struct batch* b;
		for (i = 0; i < array_size(d->d_batches); i++) {
			CHECK_LOGW(array_get(d->d_batches, i, (void**)&b))
			if (!result) {
				batch_destroy(b);
			}
		}
		array_destroy(d->d_batches);
	}
	if (d->d_free_workers) {
		queue_destroy(d->d_free_workers);
	}
	if (d->d_workers) {
		for (i = 0; i < d->d_num_workers; i++) {
			if (d->d_workers[i]) {
				free(d->d_workers[i]);
			}
		}
		free(d->d_workers);
	}

	CHECK_COND_DEST(d->d_cv);
	CHECK_LOCK_DEST(d->d_mutex);

	free(d);

	return 0;
}

/*****************************************************************************/
int dispatcher_shutdown(struct dispatcher* d) {
	int retval = 0;
	unsigned int i;
	PRE(d != NULL);

	CHECK_LOCK(d->d_mutex);
	for (i = 0; i < d->d_num_workers; i++) {
		CHECKF(worker_shutdown(d->d_workers[i]));
	}

failure:
	CHECK_UNLOCK(d->d_mutex);

	return retval;
}

/*****************************************************************************/
int dispatcher_run(struct dispatcher* d) {
	struct worker* w;
	struct batch* b;

	PRE(d != NULL);

	CHECK_LOCK(d->d_mutex);
	w = worker_create(d);
	array_get(d->d_batches, 0, (void**)&b);
	worker_assign(w, batch_next_job(b));
	CHECK_UNLOCK(d->d_mutex);
	
	worker_shutdown(w);
	worker_destroy(w);

	return 0;
}

/*****************************************************************************/
int dispatcher_add_batch(struct dispatcher* d, struct batch* b) {
	int retval = 0;

	PRE(d != NULL);
	PRE(b != NULL);

	LOG(LL_DEBUG, "adding batch %s to dispatcher %s", batch_name(b), d->d_name);

	CHECK_LOCK(d->d_mutex);
	CHECKF(array_append(d->d_batches, b));

failure:
	CHECK_UNLOCK(d->d_mutex);

	return retval;
}

/*****************************************************************************/
int dispatcher_set_workers(struct dispatcher* d, unsigned int n) {
	PRE(d != NULL);
	PRE(n > 0 && n <= DISPATCHER_WORKERS_MAX);

	CHECK_LOCK(d->d_mutex);
	d->d_num_workers = n;
	CHECK_UNLOCK(d->d_mutex);

	return 0;
}

/*****************************************************************************/
int dispatcher_notify(struct dispatcher* d, struct worker* w, struct job* j) {
	int result, retval = 0;

	PRE(d != NULL);
	PRE(w != NULL);
	PRE(j != NULL);
	PRE(j->j_state == JS_COMPLETE || j->j_state == JS_ERROR);

	CHECK_LOCK(d->d_mutex);

	CHECK_LOGW(job_destroy(j));

	CHECKF(queue_push(d->d_free_workers, w));
	d->d_num_free_workers++;

	pthread_cond_broadcast(&d->d_cv);

failure:
	CHECK_UNLOCK(d->d_mutex);

	return retval;
}
