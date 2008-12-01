#ifndef _BATCH_H_
#define _BATCH_H_

struct batch; 
struct job;

struct batch* batch_create(const char*);
void          batch_incref(struct batch*);
void          batch_decref(struct batch*);

const char*   batch_name(const struct batch*);
int           batch_add_job(struct batch*, struct job*);
unsigned int  batch_num_jobs(const struct batch*);
struct job*   batch_next_job(struct batch*);

#endif
