#ifndef _JOB_H_
#define _JOB_H_

enum job_state {
	JS_PENDING,
	JS_RUNNING,
	JS_COMPLETE
};

typedef int job_type;

struct job {
	job_type                j_type;
	enum job_state volatile j_state;
	void*                   j_data;
	unsigned int            j_id;
};

struct job* job_create(job_type);
void        job_destroy(struct job*);
int         job_run(struct job*);

/*****************************************************************************/

typedef int (*job_runner)(struct job*);
typedef void (*job_destroyer)(struct job*);

int job_register_type(const char*, job_runner, job_destroyer, job_type*);

const char* job_type_name(job_type);

#endif
