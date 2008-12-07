#ifndef _WORKER_H_
#define _WORKER_H_

struct worker;

struct dispatcher;

enum worker_state {
	WS_WAITING,
	WS_PROCESSING,
	WS_SHUTDOWN,
	WS_ERROR,
};

struct worker*    worker_create(struct dispatcher*);
void              worker_destroy(struct worker*);
int               worker_shutdown(struct worker*);

enum worker_state worker_state(struct worker*);
struct job*       worker_job(struct worker*);

#endif
