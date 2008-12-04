#include "worker.h"

#include "common.h"
#include "log.h"

#include <stdlib.h>

#define LOG_TYPE LT_WORKER

struct job;

struct worker {
	struct job* w_job;
};

struct worker* worker_create() {
	struct worker* ret;

	ret = calloc(1, sizeof(struct worker));

	return ret;
}

void worker_destroy(struct worker* w) {
	if (!w) {
		LOG(LL_WARN, "null worker");
		return;
	}

	if (w->w_job) {
		LOG(LL_WARN, "non-null job");
	}
	free(w);
}
