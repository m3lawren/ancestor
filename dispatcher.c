#include "dispatcher.h"

#include <array.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

struct dispatcher {
	char*         d_name;
	struct array* d_batches;
};

struct dispatcher* dispatcher_create(const char* name) {
	struct dispatcher* d;

	if (!(d = calloc(1, sizeof(struct dispatcher)))) {
		goto failure;
	}

	if (!(d->d_name = malloc(strlen(name) + 1))) {
		goto failure;
	}

	strcpy(d->d_name, name);

	if (!(d->d_batches = array_create())) {
		goto failure;
	}

failure:
	dispatcher_destroy(d);
	return NULL;
}

int dispatcher_destroy(struct dispatcher* d) {
	if (!d) {
		return EINVAL;
	}

	if (d->d_name) {
		free(d->d_name);
	}
	if (d->d_batches) {
		array_destroy(d->d_batches);
	}

	return 0;
}

int dispatcher_run(struct dispatcher* d) {
	if (!d) {
		return EINVAL;
	}

	return ENOTSUP;
}

int dispatcher_add_batch(struct dispatcher* d, struct batch* b) {
	if (!d) {
		return EINVAL;
	}

	if (!b) {
		return EINVAL;
	}

	return array_append(d->d_batches, b);
}