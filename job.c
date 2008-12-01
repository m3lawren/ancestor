#include "job.h"

#include <assert.h>
#include <stdlib.h>

struct job* job_create(enum job_type type) {
	struct job* j = malloc(sizeof(struct job));
	if (!j) {
		return NULL;
	}

	j->j_type = type;
	j->j_state = JS_PENDING;
	j->j_data = NULL;

	return j;
}

void job_destroy(struct job* j) {
	assert(j);
	assert(j->j_data == NULL);

	free(j);
}
