CXX := g++
LD := g++

SRC_DIR := src
RES_DIR := res
BUILD_DIR := build
BIN_DIR := bin

TARGET_EXEC := main

SRCS := $(shell find $(SRC_DIR) -name '*.cc')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

LIB_NAMES :=

INC_FLAGS := $(addprefix -I,$(SRC_DIR))

CXXFLAGS := --std=c++20 -g $(INC_FLAGS) -MMD -MP
LDFLAGS :=
LIB_FLAGS := $(addprefix -l,$(LIB_NAMES))

$(BIN_DIR)/$(TARGET_EXEC): $(OBJS)
	@mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) -o $@ $^ $(LIB_FLAGS)

$(BUILD_DIR)/%.cc.o: %.cc
	@mkdir -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

#$(BUILD_DIR)/%.c.o: %.c
#	@mkdir -p $(dir $@)
#	$(CC) -c $(CCFLAGS) -o $@ $<

.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR) $(BIN_DIR)

-include $(DEPS)
