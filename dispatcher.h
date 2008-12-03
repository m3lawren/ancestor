#ifndef _DISPATCHER_H_
#define _DISPATCHER_H_

struct dispatcher;

struct batch;

/* Basic dispatcher functions:
 *
 *  _run       - Starts the dispatcher running on the current thread
 *  _add_batch - Adds a batch to the dispatcher, must be called before the
 *               dispatcher is run. The dispatcher takes ownership of the
 *               batch, and will destroy it when it is done. Thus, it is not
 *               safe to access the batch directly from the calling function.
 */

struct dispatcher* dispatcher_create(const char* name);
int                dispatcher_destroy(struct dispatcher*);

int                dispatcher_run(struct dispatcher*);

int                dispatcher_add_batch(struct dispatcher*, struct batch*);

#endif
