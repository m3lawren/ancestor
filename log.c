#include "log.h"

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>

static enum loglevel   log_level = LL_ERROR;
static unsigned int    log_types = LT_NONE;
static FILE*           log_stream = NULL;
static pthread_mutex_t log_mutex;

int log_init(FILE* stream) {
	int ret = 0;

	if (!stream) {
		return EINVAL;
	}

	if (log_stream) {
		return EPERM;
	}

	if ((ret = pthread_mutex_init(&log_mutex, NULL))) {
		return ret;
	}

	log_stream = stream;

	return 0;
}

void log_write(unsigned int type, enum loglevel level, const char* fmt, ...) {
	va_list args;

	if (log_stream == NULL) {
		return;
	}

	assert(0 == pthread_mutex_lock(&log_mutex));

	if (level > log_level) {
		return;
	}
	if ((type & log_types) == 0) {
		return;
	}
	
	vfprintf(log_stream, fmt, args);

	assert(0 == pthread_mutex_unlock(&log_mutex));
}
