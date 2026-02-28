CC      = gcc
CFLAGS  = -Wall -Wextra -O2
TARGETS = client_ring_1 client_ring_2 client_ring_3

all: $(TARGETS)

client_ring_1: client_ring_1.c
	$(CC) $(CFLAGS) -o $@ $<

client_ring_2: client_ring_2.c
	$(CC) $(CFLAGS) -o $@ $<

client_ring_3: client_ring_3.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)