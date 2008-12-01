#ifndef _JOB_H_
#define _JOB_H_

enum job_type {
	JOB_PARSE,
	JOB_REQUEST
};

struct job {
	enum job_type j_type;
	void* data;
};

#endif
