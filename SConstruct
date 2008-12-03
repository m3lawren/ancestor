include = Dir('libm3/include')
lib = Dir('libm3/lib')

env = Environment(CPPPATH=include, LIBPATH=lib, CCFLAGS='-g -W -Wall -Werror -pedantic -std=c99')

srcs = [
	'batch.c',
	'job.c',
	'job_parse.c',
	'job_request.c',
	'log.c',
	'main.c',
	'runner.c',
]

SConscript(['libm3/SConscript'])

env.Program('ancestor', srcs, LIBS=['m3', 'pthread']);

Default('ancestor')
