#ifndef _WORKER_H_
#define _WORKER_H_

struct worker;

struct worker* worker_create();
void           worker_destroy(struct worker*);

#endif
