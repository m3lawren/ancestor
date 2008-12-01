#include "runner.h"

#include "batch.h"

#include <stdio.h>
#include <stdlib.h>

void* runner(void* arg) {
	struct batch* b = arg;
	batch_incref(b);
	printf("Thingy.\n");
	batch_decref(b);
	return NULL;
}
