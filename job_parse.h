#ifndef _JOB_PARSE_H_
#define _JOB_PARSE_H_

struct job;

struct job* job_parse_create(const char* file);
struct job* job_parse_run(struct job*);

#endif
