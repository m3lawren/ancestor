#include "jobqueue.h"

#include <assert.h>
#include <pthread.h>
#include <queue.h>
#include <stdlib.h>

struct jobqueue {
	pthread_mutex_t jq_mutex;
	struct queue *  jq_queue;
	unsigned int    jq_refs;
};

/*****************************************************************************/
static void jq_destroy(struct jobqueue* jq) {
	if (!jq) {
		return;
	}

	pthread_mutex_destroy(&jq->jq_mutex);

	if (jq->jq_queue) {

	}
}

/*****************************************************************************/
struct jobqueue* jq_create() {
	struct jobqueue* jq = malloc(sizeof(struct jobqueue));
	if (!jq) {
		return NULL;
	}

	if (!(jq->jq_queue = queue_create())) {
		free(jq);
		return NULL;
	}
	
	pthread_mutex_init(&jq->jq_mutex, NULL);

	jq_incref(jq);

	return jq;
}

/*****************************************************************************/
void jq_incref(struct jobqueue* jq) {
	assert(jq);

	pthread_mutex_lock(&jq->jq_mutex);
	jq->jq_refs++;
	pthread_mutex_unlock(&jq->jq_mutex);
}

/*****************************************************************************/
void jq_decref(struct jobqueue* jq) {
	int doit = 0;

	assert(jq);

	pthread_mutex_lock(&jq->jq_mutex);
	jq->jq_refs--;

	if (jq->jq_refs == 0) {
		doit = 1;
	}
	pthread_mutex_unlock(&jq->jq_mutex);

	if (doit) {
		jq_destroy(jq);
	}
}

/*****************************************************************************/
int jq_is_empty(struct jobqueue* jq) {
	int val;

	assert(jq);

	pthread_mutex_lock(&jq->jq_mutex);
	val = queue_is_empty(jq->jq_queue);
	pthread_mutex_unlock(&jq->jq_mutex);

	return val;
}

/*****************************************************************************/
struct job* jq_pop(struct jobqueue* jq) {
	struct job* ret;

	assert(jq);

	pthread_mutex_lock(&jq->jq_mutex);
	if (queue_is_empty(jq->jq_queue)) {
		ret = NULL;
	} else {
		ret = queue_pop(jq->jq_queue);
	}
	pthread_mutex_unlock(&jq->jq_mutex);

	return ret;
}

/*****************************************************************************/
int jq_push(struct jobqueue* jq, struct job* j) {
	int ret;

	assert(jq);
	assert(j);

	pthread_mutex_lock(&jq->jq_mutex);
	ret = queue_push(jq->jq_queue, j);
	pthread_mutex_unlock(&jq->jq_mutex);

	return ret;
}
