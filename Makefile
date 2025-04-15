# Compiler and flags
CC = gcc
TIDY = clang-tidy
CFLAGS = -O3 -Wall -I/usr/include -I/usr/local/include -I/usr/include/libmongoc-1.0 -I/usr/include/libbson-1.0 -L/usr/lib/x86_64-linux-gnu -march=native -flto -pthread

# Strategies and decks
STRATEGIES := mimic linear polynomial neural basic high-low wong
DECKS := single-deck double-deck six-shoe

# .PHONY targets (non-file targets)
.PHONY: all lint bear clean help install run run-all run-mimic-1 run-mimic-2 run-mimic-6 run-basic-1 run-basic-2 run-basic-6 run-linear-1 run-linear-2 run-linear-6 run-polynomial-1 run-polynomial-2 run-polynomial-6 run-neural-1 run-neural-2 run-neural-6 run-high-low-1 run-high-low-2 run-high-low-6 run-wong-1 run-wong-2 run-wong-6

# Directories
SRC_DIR = src
SRC_DIRS = arguments cards constants table simulator aws
INCLUDE_DIRS = $(SRC_DIRS)
OBJ_DIR = obj

# Include all directories for header files
INCLUDES = $(foreach dir, $(SRC_DIRS), -I$(SRC_DIR)/$(dir))

# Source files (from src directory)
SRC_FILES = $(wildcard $(SRC_DIR)/main.c $(foreach dir, $(SRC_DIRS), $(SRC_DIR)/$(dir)/*.c))

# Object files (place them in the obj/ directory)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

# Output binary
TARGET = bin/strikerC

# Home directory for Striker
STRIKER = ${HOME}/Striker

# Runtime parameters
HANDS ?= 500000000
THREADS ?= 24
STRATEGY ?= mimic
DECKS ?= single-deck

# Date-based log file
LOG_DIR = $(STRIKER)/Simulations/$(shell date +%Y)/$(shell date +%m)/$(shell date +%d)
LOG = $(LOG_DIR)/$(notdir $(TARGET))-$(shell date +%H%M%S).log

# Default target (run 'make help' to view available targets)
.DEFAULT_GOAL := help

# Help target to show information about the Makefile
help:
	@echo "Makefile for StrikerC project"
	@echo ""
	@echo "Available targets:"
	@echo "  all           - Compile and link all source files"
	@echo "  clean         - Remove object files and binary"
	@echo "  lint          - Run clang-tidy on the source files"
	@echo "  bear          - Generate a compile_commands.json file using Bear"
	@echo "  install       - Install the compiled binary to the Striker directory"

# Build target (compile and link the source files)
all: $(TARGET)

# Link the object files into the final binary
$(TARGET): $(OBJ_FILES)
	@mkdir -p bin
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJ_FILES) -static-libgcc -static-libstdc++ -luuid -lcjson -lcurl -L/usr/lib/x86_64-linux-gnu -lbson-1.0 -lmongoc-1.0 -march=native -flto -pthread

# Compile source files into object files in obj/ directory
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) -lgoogle-perftools $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean up object files and the binary
clean:
	rm -rf $(OBJ_DIR)/*.o $(OBJ_DIR)/*/*.o $(TARGET)

# Lint: Run clang-tidy on all source files
lint:
	$(TIDY) $(SRC_FILES)

# Generate compile_commands.json using Bear
bear:
	bear -- make clean all

# Install the compiled binary to the Striker directory
install:
	cp -rf $(TARGET) $(STRIKER)/bin

include Makefile.run

