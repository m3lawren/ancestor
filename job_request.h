#ifndef _JOB_REQUEST_H_
#define _JOB_REQUEST_H_

struct batch;
struct job;

struct job* job_request_create(unsigned int id);
void        job_request_run(struct job*, struct batch*);

#endif
