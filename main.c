#include "batch.h"
#include "log.h"
#include "runner.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define POOL_SIZE 5

int main(void) {
	pthread_t threads[POOL_SIZE];
	struct batch* b;
	int i, ret;

	log_init(stderr);
	
	b = batch_create("dummy");

	for (i = 0; i < POOL_SIZE; i++) {
		ret = pthread_create(&threads[i], NULL, runner, b);
		if (ret != 0) {
			perror("pthread_create");
			exit(EXIT_FAILURE);
		}
	}

	for (i = 0; i < POOL_SIZE; i++) {
		ret = pthread_join(threads[i], NULL);
		if (ret != 0) {
			perror("pthread_join");
			exit(EXIT_FAILURE);
		}
	}
	
	batch_decref(b);

	return EXIT_SUCCESS;
}
