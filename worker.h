#ifndef _WORKER_H_
#define _WORKER_H_

struct worker;

enum worker_state {
	WS_WAITING,
	WS_PROCESSING,
};

struct worker*    worker_create();
void              worker_destroy(struct worker*);
enum worker_state worker_state(struct worker*);
int               worker_shutdown(struct worker*);

#endif
