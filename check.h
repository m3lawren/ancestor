#ifndef _CHECK_H_
#define _CHECK_H_

#include <errno.h>
#include <pthread.h>
#include <string.h>

#define CHECK_CALL(call) if ((result = call))
#define _LOG_CALL(level, call) LOG(level, #call ": %s", strerror(result))
#define LOG_CALL_ERROR(call) _LOG_CALL(LL_ERROR, call)
#define LOG_CALL_WARN(call) _LOG_CALL(LL_WARN, call)

#define _CHECK_AND(call, action) { int _result; _CHECK_LOG(call, LL_ERROR, _result); if (_result != 0) { action; } }
#define CHECK(call) _CHECK_AND(call, return _result)
#define CHECKN(call) _CHECK_AND(call, return NULL)
#define CHECKF(call) _CHECK_AND(call, goto failure)

#define _CHECK_LOG(call, type, var) if (((var) = (call))) { LOG(type, #call ": %s", strerror(var)); }
#define CHECK_LOGW(call) _CHECK_LOG(call, LL_WARN, result)
#define CHECK_LOGE(call) _CHECK_LOG(call, LL_ERROR, result)

#define _PRE(x, action) if (!(x)) { LOG(LL_ERROR, "precondition failed: " #x); action; }
#define PRE(x) _PRE(x, return EINVAL)
#define PREG(x) _PRE(x, retval = EINVAL; goto failure)
#define PREN(x) _PRE(x, return NULL)
#define PREV(x) _PRE(x, return)

#define CHECK_LOCK(lock) CHECK(pthread_mutex_lock(&(lock)))
#define CHECK_UNLOCK(lock) CHECK(pthread_mutex_unlock(&(lock)))
#define CHECK_LOCK_INIT(lock) CHECKF(pthread_mutex_init(&(lock), NULL))
#define CHECK_COND_INIT(cond) CHECKF(pthread_cond_init(&(cond), NULL))
#define CHECK_LOCK_DEST(lock) CHECK_LOGW(pthread_mutex_destroy(&(lock)))
#define CHECK_COND_DEST(cond) CHECK_LOGW(pthread_cond_destroy(&(cond)))

#endif
