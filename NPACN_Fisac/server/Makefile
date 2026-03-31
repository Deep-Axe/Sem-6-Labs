# ── Compiler & flags ──────────────────────────────────────────────────────────
CC     = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c99 -g -Iinclude -Ivendor
LIBS   = -lsqlite3 -lssl -lcrypto -lrt -lpthread -lm

# ── Sources ────────────────────────────────────────────────────────────────────
SRCS = src/main.c src/websocket.c src/device_manager.c \
       src/auth.c src/db.c src/logger.c src/utils.c    \
       vendor/cJSON.c

TARGET = server

# ── Default target ─────────────────────────────────────────────────────────────
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LIBS)

# ── Clean ──────────────────────────────────────────────────────────────────────
clean:
	rm -f $(TARGET) smarthome.db

.PHONY: clean
