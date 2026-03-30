CC      = gcc
CFLAGS  = -Wall -Wextra -O2
TARGETS = client_bus_1 client_bus_2 client_bus_3

all: $(TARGETS)

client_bus_1: client_bus_1.c
	$(CC) $(CFLAGS) -o $@ $<

client_bus_2: client_bus_2.c
	$(CC) $(CFLAGS) -o $@ $<

client_bus_3: client_bus_3.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)