CXX := g++
LD := g++

SRC_DIR := src
TEST_DIR := test
BUILD_DIR := build
BIN_DIR := bin

SRCS := $(shell find $(SRC_DIR) -name '*.cc')
PROD_OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
PROD_DEPS := $(PROD_OBJS:.o=.d)
DEBUG_OBJS := $(SRCS:%=$(BUILD_DIR)/%.DEBUG.o)
DEBUG_DEPS := $(DEBUG_OBJS:.o=.d)

INC_FLAGS := $(addprefix -I,$(SRC_DIR))

CXXFLAGS := --std=c++20 -g $(INC_FLAGS) -MMD -MP

all: prod debug
prod: $(BIN_DIR)/main
debug: $(BIN_DIR)/main.DEBUG

$(BIN_DIR)/main: $(PROD_OBJS)
	@mkdir -p $(dir $@)
	$(LD) -o $@ $^

$(BIN_DIR)/main.DEBUG: $(DEBUG_OBJS)
	@mkdir -p $(dir $@)
	$(LD) -o $@ $^

$(BUILD_DIR)/%.cc.o: %.cc
	@mkdir -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

$(BUILD_DIR)/%.cc.DEBUG.o: %.cc
	@mkdir -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) -DDEBUG -o $@ $<

.PHONY: all clean prod debug
clean:
	@rm -rf $(BUILD_DIR) $(BIN_DIR)

-include $(DEPS)
