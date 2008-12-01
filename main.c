#include "batch.h"
#include "log.h"
#include "runner.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define POOL_SIZE 5

#define LOG_TYPE LT_MAIN

int main(void) {
	pthread_t threads[POOL_SIZE];
	struct batch* b;
	int i, ret;

	log_init(stderr);
	
	b = batch_create("dummy");

	LOG(LL_INFO, "spawning runners");

	for (i = 0; i < POOL_SIZE; i++) {
		struct runner_info* ri = malloc(sizeof(struct runner_info));
		if (!ri) {
			LOG(LL_ERROR, "unable to allocate space for a new runner");
			exit(EXIT_FAILURE);
		}
		ri->ri_batch = b;
		ri->ri_id = i;

		LOG(LL_DEBUG, "spawning runner thread #%d", ri->ri_id);

		ret = pthread_create(&threads[i], NULL, runner, ri);
		if (ret != 0) {
			LOG(LL_ERROR, "pthread_create: %s", strerror(ret));
			exit(EXIT_FAILURE);
		}
	}

	LOG(LL_INFO, "done spawning runners");

	for (i = 0; i < POOL_SIZE; i++) {
		ret = pthread_join(threads[i], NULL);
		if (ret != 0) {
			LOG(LL_ERROR, "pthread_join: %s", strerror(ret));
			exit(EXIT_FAILURE);
		}
	}

	LOG(LL_INFO, "all runners have terminated");
	
	batch_decref(b);

	return EXIT_SUCCESS;
}
