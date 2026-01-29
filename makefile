CC = gcc
AR = ar

CFLAGS  = -std=c17 -Wall -Wextra -Wpedantic -g -fsanitize=address
LDFLAGS = -fsanitize=address

SRC_DIR   = src
TEST_DIR  = tests
BUILD_DIR = build
INC_DIR   = include

LIB   = $(BUILD_DIR)/libm2l.a
BIN   = m2l
TESTS = tm2l

SRC_FILES  := $(shell find $(SRC_DIR)  -name '*.c')
TEST_FILES := $(shell find $(TEST_DIR) -name '*.c')

SRC_OBJS  := $(SRC_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/src/%.o)
TEST_OBJS := $(TEST_FILES:$(TEST_DIR)/%.c=$(BUILD_DIR)/tests/%.o)

all: $(BIN) $(TESTS)

$(LIB): $(SRC_OBJS)
	@mkdir -p $(dir $@)
	$(AR) rcs $@ $^

$(BIN): $(SRC_DIR)/main.c $(LIB)
	$(CC) -o $@ $^ $(LDFLAGS)

$(TESTS): $(TEST_OBJS) $(LIB)
	$(CC) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/src/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c -o $@ $<

$(BUILD_DIR)/tests/%.o: $(TEST_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c -o $@ $<

clean:
	rm -rf $(BUILD_DIR) $(BIN) $(TESTS)

.PHONY: all clean
