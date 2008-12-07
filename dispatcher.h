#ifndef _DISPATCHER_H_
#define _DISPATCHER_H_

struct dispatcher;

struct batch;
struct worker;

#define DISPATCHER_WORKERS     5
#define DISPATCHER_WORKERS_MAX 32

/* Basic dispatcher functions:
 *
 *  _run         - Starts the dispatcher running on the current thread
 *  _add_batch   - Adds a batch to the dispatcher, must be called before the
 *                 dispatcher is run. The dispatcher takes ownership of the
 *                 batch, and will destroy it when it is done. Thus, it is not
 *                 safe to access the batch directly from the calling function.
 *  _set_workers - Sets the number of worker threads to use. This must be set
 *                 before the dispatcher is run. Defaults to DISPATCHER_WORKERS
 *                 and cannot be more than DISPATCHER_WORKERS_MAX.
 *  _notify      - Notifies the dispatcher that a specific worker has finished
 *                 its job. This should be called by the worker thread itself,
 *                 as it is assumed that the worker's lock is held by the 
 *                 calling thread.
 */
struct dispatcher* dispatcher_create(const char* name);
int                dispatcher_destroy(struct dispatcher*);

int                dispatcher_run(struct dispatcher*);
int                dispatcher_set_workers(struct dispatcher*, unsigned int);
int                dispatcher_add_batch(struct dispatcher*, struct batch*);

int                dispatcher_notify(struct dispatcher*, struct worker*);

#endif
