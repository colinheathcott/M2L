CC = gcc
CFLAGS = -std=c17 -Wall -Wextra -Wpointer-arith -Wcast-align -Wcast-qual -Wswitch-enum -Wpedantic -Wnull-dereference -Wdouble-promotion -Wconversion -g

SRC_DIR   = src
BUILD_DIR = build

FILES := $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/**/*.c)
OBJECTS := $(FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

BINARY = smpl

all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(CC) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: all clean

clean:
	rm -rf $(BUILD_DIR) $(BINARY)