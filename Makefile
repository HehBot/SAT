CXX := g++
LD := g++

SRC_DIR := src
TEST_DIR := test
BUILD_DIR := build
BIN_DIR := bin

SRCS := $(shell find $(SRC_DIR) -name '*.cc')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d) $(BUILD_DIR)/sudoku/sudoku.cc.d $(BUILD_DIR)/sat/sat.cc.d

INC_FLAGS := $(addprefix -I,$(SRC_DIR))

LDFLAGS := -g -O3
CXXFLAGS := --std=c++20 $(INC_FLAGS) -MMD -MP -g -O3

all: sat sudoku

sudoku: $(BIN_DIR)/sudoku
sat: $(BIN_DIR)/sat

$(BIN_DIR)/sudoku: $(OBJS) $(BUILD_DIR)/sudoku/sudoku.cc.o
	@mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) -o $@ $^

$(BIN_DIR)/sat: $(OBJS) $(BUILD_DIR)/sat/sat.cc.o
	@mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/%.cc.o: %.cc
	@mkdir -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

.PHONY: all clean sat sudoku
clean:
	@rm -rf $(BUILD_DIR) $(BIN_DIR)

-include $(DEPS)
