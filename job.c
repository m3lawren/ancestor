#include "job.h"

#include "check.h"
#include "log.h"

#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define LOG_TYPE LT_JOB

#define JOB_TYPE_MAX 32
#define JOB_TYPE_NAME_MAX 32

struct job_type_info {
	char          jt_name[JOB_TYPE_NAME_MAX];
	job_runner    jt_runner;
	job_destroyer jt_destroyer;
};

static struct job_type_info job_types[32];
static int volatile         job_type_num = 0;
static pthread_mutex_t      job_type_mutex = PTHREAD_MUTEX_INITIALIZER;

/*****************************************************************************/
struct job* job_create(job_type type) {
	struct job* j;

	PREN(type >= 0 && type < job_type_num);

	LOG(LL_DEBUG, "creating new job with type %s (%d)", job_type_name(type), type);

	if (!(j = calloc(1, sizeof(struct job)))) {
		LOG(LL_ERROR, "unable to allocate memory for job");
		return NULL;
	}

	j->j_type = type;
	j->j_state = JS_PENDING;

	return j;
}

/*****************************************************************************/
void job_destroy(struct job* j) {
	PREV(j != NULL);

	if (j->j_state != JS_COMPLETE) {
		LOG(LL_WARN, "tried to destroy job in non-complete state");
	}

	LOG(LL_DEBUG, "destroying job %d of type %s (%d)", j->j_id, job_type_name(j->j_type), j->j_type);

	job_types[j->j_type].jt_destroyer(j);

	free(j);
}

/*****************************************************************************/
int job_run(struct job* j) {
	PRE(j != NULL);
	PRE(j->j_state != JS_PENDING);

	j->j_state = JS_RUNNING;
	
	return job_types[j->j_type].jt_runner(j);
}

/*****************************************************************************/
int job_register_type(const char* name, job_runner runner, job_destroyer destroyer, job_type* ret) {
	int result, retval;
	*ret = -1;

	CHECK_LOCK(job_type_mutex);

	PREG(job_type_num < JOB_TYPE_MAX);
	PREG(strlen(name) < JOB_TYPE_NAME_MAX - 1);
	PREG(runner != NULL && destroyer != NULL);

	*ret = job_type_num++;
	strcpy(job_types[*ret].jt_name, name);
	job_types[*ret].jt_runner = runner;
	job_types[*ret].jt_destroyer = destroyer;

	CHECK_UNLOCK(job_type_mutex);

	return 0;

failure:
	CHECK_UNLOCK(job_type_mutex);

	return retval;
}

/*****************************************************************************/
const char* job_type_name(job_type jt) {
	PREN(jt >= 0 && jt < job_type_num);
	return job_types[jt].jt_name;
}
