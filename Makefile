CC=gcc
CFLAGS=-Wall -O2
PROGS=broker

all: $(PROGS)

debug: clean all

broker: 
	$(CC) $@.c $(CFLAGS)  -o $@
clean:
	rm -f $(PROGS)