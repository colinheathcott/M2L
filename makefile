CC = gcc
CFLAGS = -std=c17 -Wall -Wextra -Wpointer-arith -Wcast-align -Wcast-qual -Wswitch-enum -Wpedantic -Wnull-dereference -Wdouble-promotion -Wconversion -g
LDFLAGS = -fsanitize=address

SRC_DIR   = src
BUILD_DIR = build

FILES := $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/**/*.c)
OBJECTS := $(FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

BINARY = m2l

all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)

.PHONY: all clean

clean:
	rm -rf $(BUILD_DIR) $(BINARY)