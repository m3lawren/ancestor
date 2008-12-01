#ifndef _JOB_QUEUE_H_
#define _JOB_QUEUE_H_

struct jobqueue;
struct job;

struct jobqueue* jq_create();
void             jq_incref(struct jobqueue*);
void             jq_decref(struct jobqueue*);
int              jq_is_empty(struct jobqueue*);
struct job*      jq_pop(struct jobqueue*);
int              jq_push(struct jobqueue*, struct job*);

#endif
