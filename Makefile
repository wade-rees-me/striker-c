# Compiler and flags
CC = gcc
CFLAGS = -O3 -Wall -I/usr/include -I/usr/local/include -I/usr/include/libmongoc-1.0 -I/usr/include/libbson-1.0 -L/usr/lib/x86_64-linux-gnu -march=native -flto -pthread

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

#
STRIKER = /home/wade/Striker

# Default target
all: $(TARGET)

# Build target
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

# Install
install:
	cp -rf $(TARGET) $(STRIKER)/bin

