#ifndef _LOG_H_
#define _LOG_H_

#define LT_MAIN   0x01
#define LT_RUNNER 0x02
#define LT_JOB    0x04
#define LT_BATCH  0x08

#define LT_NONE   0x0
#define LT_ALL    0xffffffff

#include <stdio.h>

enum loglevel {
	LL_ERROR = 0,
	LL_INFO  = 1,
	LL_DEBUG = 2
};

int  log_init(FILE* stream);
void log_write(unsigned int type, enum loglevel level, const char* fmt, ...);

#endif
