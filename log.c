#include "log.h"

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>

static enum loglevel   log_level = LL_INFO;
static unsigned int    log_types = LT_ALL;
static FILE*           log_stream = NULL;
static pthread_mutex_t log_mutex;

static const char* ll_string(enum loglevel level) {
	switch (level) {
		case LL_ERROR:
			return "ERROR";
		case LL_INFO:
			return "INFO ";
		case LL_DEBUG:
			return "DEBUG";
		default:
			return "";
	}
}

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

void _log_write(unsigned int type, enum loglevel level, const char* func, const char* fmt, ...) {
	va_list args;

	if (log_stream == NULL) {
		return;
	}

	assert(0 == pthread_mutex_lock(&log_mutex));

	if (level <= log_level && (type && log_types) != 0) {
		fprintf(log_stream, "[%s] %s: ", ll_string(level), func);
		va_start(args, fmt);
		vfprintf(log_stream, fmt, args);
		va_end(args);
		fputc('\n', log_stream);
	}

	assert(0 == pthread_mutex_unlock(&log_mutex));
}
