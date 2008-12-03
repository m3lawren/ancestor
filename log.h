#ifndef _LOG_H_
#define _LOG_H_

#define LT_MAIN       0x01
#define LT_RUNNER     0x02
#define LT_JOB        0x04
#define LT_BATCH      0x08
#define LT_DISPATCHER 0x10

#define LT_NONE   0x0
#define LT_ALL    0xffffffff

#include <stdio.h>

enum loglevel {
	LL_ERROR = 0,
	LL_INFO  = 1,
	LL_DEBUG = 2
};

int  log_init(FILE* stream);
#define log_write(type, level, ...) _log_write(type, level, __func__, __VA_ARGS__)
void _log_write(unsigned int type, enum loglevel level, const char* func, const char* fmt, ...);

#define LOG(level, ...) log_write(LOG_TYPE, level, __VA_ARGS__)

#endif
