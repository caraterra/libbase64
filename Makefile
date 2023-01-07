# This Makefile will produce the static library lib[NAME].a
NAME=base64

CFLAGS=-Wall -Wextra -Wpedantic -std=c99
CPPFLAGS=
LDFLAGS=
LDLIBS=

######## DO NOT MODIFY BELOW THIS LINE ########

SHELL=/bin/sh
PYTHON:=$(shell command -v python3)
RUBY:=$(shell command -v ruby)

.SUFFIXES:

LIB=lib$(NAME).a
INC=$(NAME).h

BUILD_DIR=build

INC_DIR=$(BUILD_DIR)/include
INC_PATH=$(INC_DIR)/$(INC)

LIB_DIR=$(BUILD_DIR)/lib
LIB_PATH=$(LIB_DIR)/$(LIB)

OBJ_DIR=$(BUILD_DIR)/obj
OBJ=$(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

SRC_DIR=src
SRC=$(wildcard $(SRC_DIR)/*.c)

TEST_DIR=test
TEST_BUILD_DIR=$(TEST_DIR)/$(BUILD_DIR)
TEST_BIN_DIR=$(TEST_BUILD_DIR)/bin
TEST_BIN=$(TEST_SRC:$(TEST_SRC_DIR)/%.c=$(TEST_BIN_DIR)/%)
TEST_OBJ_DIR=$(TEST_BUILD_DIR)/obj
TEST_RESULT_DIR=$(TEST_BUILD_DIR)/result
TEST_RESULT=$(TEST_BIN:$(TEST_BIN_DIR)/%=$(TEST_RESULT_DIR)/%.testresults)
TEST_SRC_DIR=$(TEST_DIR)/$(SRC_DIR)
TEST_SRC=$(wildcard $(TEST_SRC_DIR)/test_*.c)

UNITY_DIR=unity
UNITY_AUTO_DIR=$(UNITY_DIR)/auto
UNITY_SRC_DIR=$(UNITY_DIR)/src
UNITY_SRC=$(wildcard $(UNITY_SRC_DIR)/*.c)
UNITY_INC=$(wildcard $(UNITY_SRC_DIR)/*.h)
UNITY_OBJ=$(UNITY_SRC:$(UNITY_SRC_DIR)/%.c=$(TEST_OBJ_DIR)/%.o)

all: $(INC_PATH) $(LIB_PATH)

$(INC_DIR):
	mkdir -p $@

$(LIB_DIR):
	mkdir -p $@

$(OBJ_DIR):
	mkdir -p $@

$(TEST_BIN_DIR):
	mkdir -p $@

$(TEST_OBJ_DIR):
	mkdir -p $@

$(TEST_RESULT_DIR):
	mkdir -p $@

$(INC_PATH): $(SRC_DIR)/$(INC) | $(INC_DIR)
	cp $< $(INC_DIR)

$(LIB_PATH): $(OBJ) | $(LIB_DIR)
	$(AR) -rcs $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(TEST_BIN_DIR)/%: $(TEST_OBJ_DIR)/%.o $(UNITY_OBJ) | $(INC_PATH) $(LIB_PATH) $(TEST_BIN_DIR)
	$(CC) -L$(LIB_DIR) $(LDFLAGS) $^ $(LDLIBS) -l$(NAME) -o $@

$(TEST_OBJ_DIR)/%.o: $(TEST_SRC_DIR)/%.c | $(TEST_OBJ_DIR)
	$(CC) $(CFLAGS) -I $(UNITY_SRC_DIR) -I $(INC_DIR) $(CPPFLAGS) -c $< -o $@

$(TEST_OBJ_DIR)/%.o: $(UNITY_SRC_DIR)/%.c | $(TEST_OBJ_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(TEST_BUILD_DIR)

check: $(TEST_RESULT)
ifneq ($(PYTHON),)
	@$(PYTHON) $(UNITY_AUTO_DIR)/unity_test_summary.py $(TEST_RESULT_DIR) .
else ifneq ($(RUBY),)
	@$(RUBY) $(UNITY_AUTO_DIR)/unity_test_summary.rb $(TEST_RESULT_DIR) .
else
	@cat $(TEST_RESULT)
endif

$(TEST_RESULT_DIR)/%.testresults: $(TEST_BIN_DIR)/% | $(TEST_RESULT_DIR)
	-./$< > $@ 2>&1

.PHONY: all clean check
.PRECIOUS: $(TEST_OBJ_DIR)/%.o $(TEST_BIN_DIR)/%
