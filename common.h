#ifndef _COMMON_H_
#define _COMMON_H_

#define _CHECK_LOCK_JUMP(func, lock, jump) if ((result = func(&(lock)))) { LOG(LL_ERROR, #func ": %s", strerror(result)); jump; }
#define _CHECK_LOCK_RETURN(func, lock)  _CHECK_LOCK_JUMP(func, lock, return result)
#define CHECK_LOCK(lock) _CHECK_LOCK_RETURN(pthread_mutex_lock, lock)
#define CHECK_UNLOCK(lock) _CHECK_LOCK_RETURN(pthread_mutex_unlock, lock)
#define CHECK_LOCK_GOTO(lock) _CHECK_LOCK_JUMP(pthread_mutex_lock, lock, goto failure)
#define CHECK_UNLOCK_GOTO(lock) _CHECK_LOCK_JUMP(pthread_mutex_unlock, lock, goto failure)

#define CHECK_COND(cond) if (!(cond)) { LOG(LL_ERROR, "condition failed: " #cond); return EINVAL; }
#define CHECK_NULL(var) CHECK_COND((var) != NULL)

#endif
