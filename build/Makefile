CXX = /opt/homebrew/opt/llvm/bin/clang++
LLVM_CONFIG = /opt/homebrew/opt/llvm/bin/llvm-config

LLVM_CXXFLAGS := $(shell $(LLVM_CONFIG) --cxxflags)
LLVM_LDFLAGS  := $(shell $(LLVM_CONFIG) --ldflags --system-libs --libs all)
CLANG_LIBS    := $(shell $(LLVM_CONFIG) --libs all | sed 's/-lLLVM[^ ]*//g')
CLANG_SYSTEM  := $(shell $(LLVM_CONFIG) --system-libs)

SRC_ROOT = ../src
INCLUDE_ROOT = ../include

BIN_DIR = bin
OBJ_DIR = obj

CXXFLAGS = -std=c++17 -O2 $(LLVM_CXXFLAGS) -I$(INCLUDE_ROOT) -Wall -Wextra -pedantic
LDFLAGS = $(LLVM_LDFLAGS) $(CLANG_LIBS) $(CLANG_SYSTEM)

LOOP_TARGET = $(BIN_DIR)/loop-analyzer
STRUCT_TARGET = $(BIN_DIR)/struct-analyzer
UTILS_TARGET = $(BIN_DIR)/utils-tests

COMMON_OBJS = \
	$(OBJ_DIR)/utils/Struct.o \
	$(OBJ_DIR)/utils/CacheInformation.o

LOOP_OBJS = \
	$(OBJ_DIR)/loop_analysis_ast/main.o \
	$(OBJ_DIR)/loop_analysis_ast/Loop.o \
	$(COMMON_OBJS)

STRUCT_OBJS = \
	$(OBJ_DIR)/struct_analysis/main.o \
	$(OBJ_DIR)/struct_analysis/StructAnalysis.o \
	$(OBJ_DIR)/utils/Struct.o

UTILS_OBJS = \
	$(OBJ_DIR)/utils/CacheUnitTests.o \
	$(OBJ_DIR)/utils/CacheInformation.o

all: directories loop struct utils

directories:
	mkdir -p $(BIN_DIR)
	mkdir -p $(OBJ_DIR)/loop_analysis_ast
	mkdir -p $(OBJ_DIR)/struct_analysis
	mkdir -p $(OBJ_DIR)/utils

loop: $(LOOP_TARGET)

$(LOOP_TARGET): $(LOOP_OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS) -lclang-cpp

$(OBJ_DIR)/loop_analysis_ast/main.o: ../tests/loop_analysis_ast/main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/loop_analysis_ast/Loop.o: $(SRC_ROOT)/loop_analysis_ast/Loop.cpp $(INCLUDE_ROOT)/loop_analysis_ast/Loop.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

struct: $(STRUCT_TARGET)

$(STRUCT_TARGET): $(STRUCT_OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/struct_analysis/main.o: ../tests/struct_analysis/main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/struct_analysis/StructAnalysis.o: $(SRC_ROOT)/struct_analysis/StructAnalysis.cpp $(INCLUDE_ROOT)/struct_analysis/StructAnalysis.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

utils: $(UTILS_TARGET)

$(UTILS_TARGET): $(UTILS_OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/utils/CacheUnitTests.o: ../tests/utils/CacheUnitTests.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/utils/Struct.o: $(SRC_ROOT)/utils/Struct.cpp $(INCLUDE_ROOT)/utils/Struct.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/utils/CacheInformation.o: $(SRC_ROOT)/utils/CacheInformation.cpp $(INCLUDE_ROOT)/utils/CacheInformation.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

clean_objects:
	rm -rf $(OBJ_DIR)

.PHONY: all directories loop struct utils clean clean_objects