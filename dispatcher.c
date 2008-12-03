#include "dispatcher.h"

#include "batch.h"
#include "common.h"
#include "log.h"

#include <array.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define LOG_TYPE LT_DISPATCHER

struct dispatcher {
	char*         d_name;
	struct array* d_batches;
	unsigned int  d_num_workers;
};

/*****************************************************************************/
struct dispatcher* dispatcher_create(const char* name) {
	struct dispatcher* d;

	if (!(d = calloc(1, sizeof(struct dispatcher)))) {
		LOG(LL_ERROR, "unable to allocate space for struct");
		goto failure;
	}

	if (!(d->d_name = malloc(strlen(name) + 1))) {
		LOG(LL_ERROR, "unable to allocate space for name");
		goto failure;
	}

	strcpy(d->d_name, name);

	if (!(d->d_batches = array_create())) {
		LOG(LL_ERROR, "unable to allocate space for batches");
		goto failure;
	}

	d->d_num_workers = DISPATCHER_WORKERS;

	return d;
failure:
	dispatcher_destroy(d);
	return NULL;
}

/*****************************************************************************/
int dispatcher_destroy(struct dispatcher* d) {
	CHECK_NULL(d);

	if (d->d_name) {
		free(d->d_name);
	}
	if (d->d_batches) {
		array_destroy(d->d_batches);
	}

	return 0;
}

/*****************************************************************************/
int dispatcher_run(struct dispatcher* d) {
	CHECK_NULL(d);

	return ENOTSUP;
}

/*****************************************************************************/
int dispatcher_add_batch(struct dispatcher* d, struct batch* b) {
	CHECK_NULL(d);
	CHECK_NULL(b);

	LOG(LL_DEBUG, "adding batch %s to dispatcher %s", batch_name(b), d->d_name);

	return array_append(d->d_batches, b);
}

/*****************************************************************************/
int dispatcher_set_workers(struct dispatcher* d, unsigned int n) {
	CHECK_NULL(d);
	CHECK_COND(n > 0 && n <= DISPATCHER_WORKERS_MAX);
	
	d->d_num_workers = n;

	return 0;
}
