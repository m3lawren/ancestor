#include "job.h"

#include "log.h"

#include <assert.h>
#include <stdlib.h>

#define LOG_TYPE LT_JOB

const char* job_type_string(enum job_type type) {
	switch (type) {
		case JT_PARSE:
			return "JT_PARSE";
		case JT_REQUEST:
			return "JT_REQUEST";
		default:
			return "";
	}
}

struct job* job_create(enum job_type type) {
	struct job* j = calloc(sizeof(struct job));

	LOG(LL_DEBUG, "creating new job with type %s", job_type_string(type));

	if (!j) {
		LOG(LL_ERROR, "unable to allocate memory for job");
		return NULL;
	}

	j->j_type = type;
	j->j_state = JS_PENDING;

	return j;
}

void job_destroy(struct job* j) {
	assert(j);
	assert(j->j_data == NULL);

	LOG(LL_DEBUG, "destroying job %d", j->j_id);

	free(j);
}
