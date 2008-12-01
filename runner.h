#ifndef _RUNNER_H_
#define _RUNNER_H_

struct runner_info {
	struct batch* ri_batch;
	unsigned int  ri_id;
};

void* runner(void*);

#endif
