#include "dispatcher.h"

#include "batch.h"
#include "check.h"
#include "log.h"

#include <array.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define LOG_TYPE LT_DISPATCHER

struct dispatcher {
	char*           d_name;
	struct array*   d_batches;
	unsigned int    d_num_workers;
	pthread_cond_t  d_cv;
	pthread_mutex_t d_mutex;
	struct worker** d_workers;
};

/*****************************************************************************/
struct dispatcher* dispatcher_create(const char* name) {
	int result;
	struct dispatcher* d;

	if (!(d = malloc(sizeof(struct dispatcher)))) {
		LOG(LL_ERROR, "unable to allocate space for struct");
		goto failure;
	}

	d->d_name = NULL;
	d->d_batches = NULL;
	d->d_num_workers = DISPATCHER_WORKERS;
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

	if (d->d_name) {
		free(d->d_name);
	}
	if (d->d_batches) {
		struct batch* b;
		for (i = 0; i < array_size(d->d_batches); i++) {
			CHECK_CALL(array_get(d->d_batches, i, (void**)&b)) {
				LOG_CALL_WARN(array_get);
			} else {
				batch_destroy(b);
			}
		}
		array_destroy(d->d_batches);
	}
	if (d->d_workers) {
		for (i = 0; i < d->d_num_workers; i++) {
			if (d->d_workers[i]) {
				free(d->d_workers[i]);
			}
		}
		free(d->d_workers);
	}

	pthread_cond_destroy(&d->d_cv);
	pthread_mutex_destroy(&d->d_mutex);

	free(d);

	return 0;
}

/*****************************************************************************/
int dispatcher_run(struct dispatcher* d) {
	PRE(d != NULL);

	return ENOTSUP;
}

/*****************************************************************************/
int dispatcher_add_batch(struct dispatcher* d, struct batch* b) {
	int result;

	PRE(d != NULL);
	PRE(b != NULL);

	LOG(LL_DEBUG, "adding batch %s to dispatcher %s", batch_name(b), d->d_name);

	CHECK_LOCK(d->d_mutex);
	result = array_append(d->d_batches, b);
	CHECK_UNLOCK(d->d_mutex);

	return result;
}

/*****************************************************************************/
int dispatcher_set_workers(struct dispatcher* d, unsigned int n) {
	int result;

	PRE(d != NULL);
	PRE(n > 0 && n <= DISPATCHER_WORKERS_MAX);

	CHECK_LOCK(d->d_mutex);
	d->d_num_workers = n;
	CHECK_UNLOCK(d->d_mutex);

	return 0;
}
