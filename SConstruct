include = Dir('libm3/include')
lib = Dir('libm3/lib')

env = Environment(CPPPATH=include, LIBPATH=lib, CCFLAGS='-g -W -Wall -Werror -pedantic -std=c99')

srcs = [
	'batch.c',
	'dispatcher.c',
	'job.c',
	'job_dummy.c',
	'log.c',
	'main.c',
]

SConscript(['libm3/SConscript'])

env.Program('ancestor', srcs, LIBS=['m3', 'pthread']);

Default('ancestor')

import os
os.system('ctags -R *')
