CC      = gcc
CFLAGS  = -Wall -Wextra -Wpedantic -std=c99 -g
LIBS    = -lsqlite3 -lssl -lcrypto -lpthread -lrt
TARGET  = server
SRCS    = main.c websocket.c device_manager.c auth.c db.c logger.c utils.c

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LIBS)

clean:
	rm -f $(TARGET)
