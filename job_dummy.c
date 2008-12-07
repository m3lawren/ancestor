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

		CHECK_LOGE(job_register_type("dummy", dummy_runner, dummy_destroyer, &dummy_type));
		if (result == 0) {
			LOG(LL_DEBUG, "initialized with type %d", dummy_type);
		}
	}

	CHECK_UNLOCK(dummy_lock);

	return 0;
}

struct job* job_dummy_create() {
	struct job* j;
	int result;

	CHECK_LOGE(dummy_check_init());

	j = job_create(dummy_type);	
	j->j_data = NULL;

	return j;
}
