#include "batch.h"
#include "dispatcher.h"
#include "job_dummy.h"
#include "log.h"

#include <stdlib.h>
#include <string.h>

#define LOG_TYPE LT_MAIN

int main(void) {
	struct batch* b;
	struct dispatcher* d;
	int result;

	log_init(stderr);

	LOG(LL_DEBUG, "initializing objects");
	d = dispatcher_create("dtest");
	b = batch_create("btest");

	if ((result = batch_add_job(b, job_dummy_create()))) {
		LOG(LL_ERROR, "batch_add_job: %s", strerror(result));
		dispatcher_destroy(d);
		batch_decref(b);	
		return EXIT_FAILURE;
	}

	if ((result = dispatcher_add_batch(d, b))) {
		LOG(LL_ERROR, "dispatcher_add_batch: %s", strerror(result));
		dispatcher_destroy(d);
		batch_decref(b);
		return EXIT_FAILURE;
	}

	LOG(LL_DEBUG, "done initializing objects");

	LOG(LL_DEBUG, "starting dispatcher");
	if ((result = dispatcher_run(d))) {
		LOG(LL_ERROR, "dispatcher_run: %s", strerror(result));
	} else {
		LOG(LL_DEBUG, "dispatcher exited successfully");
	}

	dispatcher_destroy(d);

	return EXIT_SUCCESS;
}
