#ifndef _CHECK_H_
#define _CHECK_H_

#include <errno.h>
#include <pthread.h>
#include <string.h>

#define CHECK_CALL(call) if ((result = call))
#define _LOG_CALL(level, call) LOG(level, #call ": %s", strerror(result))
#define LOG_CALL_ERROR(call) _LOG_CALL(LL_ERROR, call)
#define LOG_CALL_WARN(call) _LOG_CALL(LL_WARN, call)

#define _CHECK_AND(call, action) CHECK_CALL(call) { LOG_CALL_ERROR(call); action; }
#define CHECK(call) _CHECK_AND(call, return result)
#define CHECKN(call) _CHECK_AND(call, return NULL)
#define CHECKF(call) _CHECK_AND(call, goto failure)

#define _PRE(x, action) if (!(x)) { LOG(LL_ERROR, "precondition failed: " #x); action; }
#define PRE(x) _PRE(x, return EINVAL)
#define PREG(x) _PRE(x, retval = result; goto failure)
#define PREN(x) _PRE(x, return NULL)
#define PREV(x) _PRE(x, return)

#define CHECK_LOCK(lock) CHECK(pthread_mutex_lock(&(lock)))
#define CHECK_UNLOCK(lock) CHECK(pthread_mutex_unlock(&(lock)))
#define CHECK_LOCK_INIT(lock) CHECKF(pthread_mutex_init(&(lock), NULL))
#define CHECK_COND_INIT(cond) CHECKF(pthread_cond_init(&(cond), NULL))

#endif
