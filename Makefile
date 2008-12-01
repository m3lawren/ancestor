CC=gcc
CFLAGS=-g -W -Wall -Werror -ansi -pedantic -MD -Ilibm3/include
LDLIBS=-lcurl -lpthread -lm3 
LDFLAGS=-Llibm3/lib/ $(LDLIBS)
BINARY=ancest
OBJS=jobqueue.o main.o
DEPS=libm3/lib/libm3.a

$(BINARY): $(OBJS) $(DEPS)
	@ctags -R *
	$(CC) -o $(BINARY) $(OBJS) $(LDFLAGS)

-include *.d

.PHONY: clean depclean

clean: 
	rm -f $(BINARY) $(OBJS) $(OBJS:%.o=%.d)

depclean: clean
	@make -C libm3 clean

libm3/lib/libm3.a:
	@make -C libm3
