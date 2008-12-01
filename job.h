#ifndef _JOB_H_
#define _JOB_H_

enum job_type {
	JT_PARSE,
	JT_REQUEST
};

enum job_state {
	JS_PENDING,
	JS_RUNNING,
	JS_COMPLETE
};

struct job {
	enum job_type           j_type;
	enum job_state volatile j_state;
	void*                   j_data;
	unsigned int            j_id;
};

struct job* job_create(enum job_type);
void        job_destroy(struct job*);

const char* job_type_string(enum job_type);

#endif
