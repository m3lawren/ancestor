#include "runner.h"

#include "batch.h"
#include "log.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define LOG_TYPE LT_RUNNER

void* runner(void* arg) {
	struct runner_info* ri = arg;
	
	batch_incref(ri->ri_batch);

	LOG(LL_INFO, "runner %u starting", ri->ri_id);
	LOG(LL_INFO, "runner %u exiting", ri->ri_id);

	batch_decref(ri->ri_batch);

	free(ri);
	return NULL;
}
