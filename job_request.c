#include "job_request.h"
#include "job.h"

#include <stdlib.h>

struct job_request {
	unsigned int jr_id;
};

struct job* job_request_create(unsigned int id) {
	struct job* j;
	struct job_request* jr;

	if (!(j = job_create(JT_REQUEST))) {
		return NULL;
	}

	if (!(jr = malloc(sizeof(struct job_request)))) {
		job_destroy(j);
		return NULL;
	}
	jr->jr_id = id;

	j->j_data = jr;

	return j;
}

struct job* job_request_run(struct job* j) {
	(void)j;
	return NULL;
}
