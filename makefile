CC = gcc
INCLUDE = .
CFLAGS = -g2 -Wall -ansi -pedantic
LIBS = -lpthread
SRCS = server.c
OBJS = $(SRCS:.c=.o)
BINS = $(SRCS:.c=)


.PHONY: clean


all: $(BINS)

$(BINS): %: %.o
	$(CC) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) -I$(INCLUDE) $(CFLAGS) -c -o $@ $<

dep:
	$(CC) -M $(SRCS) > .depend

clean:
	rm -f $(BINS) $(OBJS) .depend


sinclude .depend
