CC=gcc
CFLAGS=`curl-config --cflags` -g -W -Wall -ansi -pedantic -MD
LDFLAGS=`curl-config --libs`
BINARY=ancest
OBJS=main.o

$(BINARY): $(OBJS)
	$(CC) $(LDFLAGS) -o $(BINARY) $(OBJS)

-include *.d

.PHONY: clean

clean: 
	rm -f $(BINARY) $(OBJS) $(OBJS:%.o=%.d)
