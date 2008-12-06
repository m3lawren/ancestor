#include "job_dummy.h"

#include "check.h"
#include "job.h"
#include "log.h"

#include <pthread.h>
#include <string.h>

#define LOG_TYPE LT_JOB_DUMMY

static job_type        dummy_type = -1;
static pthread_mutex_t dummy_lock = PTHREAD_MUTEX_INITIALIZER;

static int dummy_runner(struct job* j) {
	(void)j;
	return 0;
}

static void dummy_destroyer(struct job* j) {
	(void)j;
}

static int dummy_check_init() {
	int result;

	CHECK_LOCK(dummy_lock);

	if (dummy_type < 0) {
		LOG(LL_DEBUG, "trying to initialize type");

		CHECK_CALL(job_register_type("dummy", dummy_runner, dummy_destroyer, &dummy_type)) {
			LOG(LL_ERROR, "unable to register type");
		} else {
			LOG(LL_DEBUG, "initialized with type %d", dummy_type);
		}
	}

	CHECK_UNLOCK(dummy_lock);

	return result;
}

struct job* job_dummy_create() {
	struct job* j;

	if (dummy_check_init()) {
		LOG(LL_ERROR, "type not initialized, cannot create new job");
		return NULL;
	}

	j = job_create(dummy_type);	
	j->j_data = NULL;

	return j;
}
