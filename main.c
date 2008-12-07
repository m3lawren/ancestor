#include "batch.h"
#include "check.h"
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

	if (!(d = dispatcher_create("dtest"))) {
		LOG(LL_ERROR, "dispatcher_create returned null");
		return EXIT_FAILURE;
	}
	
	if (!(b = batch_create("btest"))) {
		LOG(LL_ERROR, "batch_create returned null");
		return EXIT_FAILURE;
	}

	if ((result = batch_add_job(b, job_dummy_create()))) {
		LOG(LL_ERROR, "batch_add_job: %s", strerror(result));
		dispatcher_destroy(d);
		batch_destroy(b);	
		return EXIT_FAILURE;
	}

	CHECK_LOGE(dispatcher_add_batch(d, b));
	
	if (result != 0) {
		dispatcher_destroy(d);
		batch_destroy(b);
		return EXIT_FAILURE;
	}

	LOG(LL_DEBUG, "done initializing objects");

	LOG(LL_DEBUG, "starting dispatcher");
	CHECK_LOGE(dispatcher_run(d));

	if (result == 0) {
		LOG(LL_DEBUG, "dispatcher exited successfully");
	}

	dispatcher_destroy(d);

	return EXIT_SUCCESS;
}
