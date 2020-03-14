CC=gcc
CFLAGS=-Wall -O2
PROGS=broker

all: $(PROGS)

broker: 
	$(CC) $@.c $(CFLAGS)  -o $@

clean:
	rm -f $(PROGS)