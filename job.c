#include "job.h"

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
static int                  job_type_num = 0;
static pthread_mutex_t      job_type_mutex = PTHREAD_MUTEX_INITIALIZER;

/*****************************************************************************/
struct job* job_create(job_type type) {
	struct job* j;

	if (type < 0 || type >= job_type_num) {
		LOG(LL_ERROR, "tried to create job with unknown type %d", type);
		return NULL;
	}

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
	if (!j) {
		LOG(LL_ERROR, "tried to destroy null job");
		return;
	}

	if (j->j_state != JS_COMPLETE) {
		LOG(LL_WARN, "tried to destroy job in non-complete state");
	}

	LOG(LL_DEBUG, "destroying job %d of type %s (%d)", j->j_id, job_type_name(j->j_type), j->j_type);

	job_types[j->j_type].jt_destroyer(j);

	free(j);
}

/*****************************************************************************/
int job_run(struct job* j) {
	if (!j) {
		LOG(LL_ERROR, "tried to run null job");
		return EINVAL;
	}

	if (j->j_state != JS_PENDING) {
		LOG(LL_ERROR, "tried to run job in non-pending state");
		return EINVAL;
	}

	j->j_state = JS_RUNNING;
	
	return job_types[j->j_type].jt_runner(j);
}

/*****************************************************************************/
job_type job_register_type(const char* name, job_runner runner, job_destroyer destroyer) {
	int result;
	job_type ret;

	if ((result = pthread_mutex_lock(&job_type_mutex))) {
		LOG(LL_ERROR, "pthread_mutex_lock: %s", strerror(result));
		return -1;
	}

	if (job_type_num == JOB_TYPE_MAX) {
		LOG(LL_ERROR, "ran out of job type slots");
		goto failure;
	}

	if (strlen(name) >= JOB_TYPE_NAME_MAX - 1) {
		LOG(LL_ERROR, "job name too long: %s", name);
		goto failure;
	}

	if (runner == NULL || destroyer == NULL) {
		LOG(LL_ERROR, "null destroyer or runner encountered on type %s", name);
		goto failure;
	}

	ret = job_type_num++;
	strcpy(job_types[ret].jt_name, name);
	job_types[ret].jt_runner = runner;
	job_types[ret].jt_destroyer = destroyer;

	if ((result = pthread_mutex_unlock(&job_type_mutex))) {
		LOG(LL_ERROR, "pthread_mutex_unlock: %s", strerror(result));
		job_type_num--;
		return -1;
	}

	return ret;

failure:
	pthread_mutex_unlock(&job_type_mutex);
	return -1;
}

/*****************************************************************************/
const char* job_type_name(job_type jt) {
	if (jt < 0 || jt >= job_type_num) {
		return NULL;
	}
	return job_types[jt].jt_name;
}
