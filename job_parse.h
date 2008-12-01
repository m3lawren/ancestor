#ifndef _JOB_PARSE_H_
#define _JOB_PARSE_H_

struct batch;
struct job;

struct job* job_parse_create(const char* file);
void        job_parse_run(struct job*, struct batch*);

#endif
