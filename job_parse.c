#include "job_parse.h"
#include "job.h"

#include <stdlib.h>
#include <string.h>

struct job_parse {
	char* jp_file;
};

struct job* job_parse_create(const char* file) {
	struct job* j;
	struct job_parse* jp;

	if (!(j = job_create(JT_PARSE))) {
		return NULL;
	}

	if (!(jp = malloc(sizeof(struct job_parse)))) {
		job_destroy(j);
		return NULL;
	}

	if (!(jp->jp_file = malloc(strlen(file) + 1))) {
		free(jp);
		job_destroy(j);
		return NULL;
	}

	strcpy(jp->jp_file, file);

	j->j_data = jp;

	return j;
}

void job_parse_run(struct job* j, struct batch* b) {
	(void)j;
	(void)b;
}
