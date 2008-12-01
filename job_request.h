#ifndef _JOB_REQUEST_H_
#define _JOB_REQUEST_H_

struct job;

struct job* job_request_create(unsigned int id);
struct job* job_request_run(struct job*);

#endif
