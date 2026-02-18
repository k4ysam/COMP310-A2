CC=gcc
CFLAGS=-Wall -Wextra -g
FMT=indent

SRCS = shell.c interpreter.c shellmemory.c pcb.c queue.c scheduler.c
OBJS = $(SRCS:.c=.o)

mysh: $(OBJS)
	$(CC) $(CFLAGS) -o mysh $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

style: $(SRCS) shell.h interpreter.h shellmemory.h pcb.h queue.h scheduler.h
	$(FMT) $?

clean:
	$(RM) mysh $(OBJS) *~
