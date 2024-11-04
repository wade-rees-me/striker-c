# Compiler and flags
CC = gcc
CFLAGS = -O3 -Wall -I/usr/include -I/usr/local/include

# Directories
SRC_DIR = src
SRC_DIRS = arguments cards constants table simulator
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

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(OBJ_FILES)
	@mkdir -p bin
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJ_FILES) -luuid -lcjson -lcurl

# Compile source files into object files in obj/ directory
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean up object files and the binary
clean:
	rm -rf $(OBJ_DIR)/*.o $(OBJ_DIR)/*/*.o $(TARGET)

