#include "runner.h"

#include "batch.h"
#include "job.h"
#include "job_parse.h"
#include "job_request.h"
#include "log.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define LOG_TYPE LT_RUNNER

void* runner(void* arg) {
	struct runner_info* ri = arg;
	struct job* j;
	
	batch_incref(ri->ri_batch);

	LOG(LL_INFO, "runner %u starting", ri->ri_id);

	while ((j = batch_next_job(ri->ri_batch))) {
		switch (j->j_type) {
			case JT_PARSE:
				job_parse_run(j, ri->ri_batch);
				break;
			case JT_REQUEST:
				job_request_run(j, ri->ri_batch);
				break;
			default:
				LOG(LL_ERROR, "got unexpected type: %s (%d)", job_type_string(j->j_type), j->j_type);
				break;
		}
	}

	LOG(LL_INFO, "runner %u exiting", ri->ri_id);

	batch_decref(ri->ri_batch);

	free(ri);
	return NULL;
}
