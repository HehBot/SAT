CXX := g++
LD := g++

SRC_DIR := src
TEST_DIR := test
BUILD_DIR := build
BIN_DIR := bin

SRCS := $(shell find $(SRC_DIR) -name '*.cc')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_FLAGS := $(addprefix -I,$(SRC_DIR))

CXXFLAGS := --std=c++20 -g $(INC_FLAGS) -MMD -MP

$(BIN_DIR)/main: $(OBJS)
	@mkdir -p $(dir $@)
	$(LD) -o $@ $^

$(BUILD_DIR)/%.cc.o: %.cc
	@mkdir -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR) $(BIN_DIR)

-include $(DEPS)
