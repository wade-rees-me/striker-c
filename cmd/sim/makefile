# Compiler and flags
CC = gcc
CFLAGS = -Wall -I/usr/include -I/usr/local/include

# Directories
SRC_DIRS = arguments cards constants rules simulator
INCLUDE_DIRS = $(SRC_DIRS)

# Include all directories for header files
INCLUDES = $(foreach dir, $(INCLUDE_DIRS), -I$(dir))

# Source files
SRC_FILES = $(wildcard main.c $(foreach dir, $(SRC_DIRS), $(dir)/*.c))

# Object files
OBJ_FILES = $(SRC_FILES:.c=.o)

# Output binary
TARGET = bin/striker

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(OBJ_FILES)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJ_FILES) -luuid -lcjson -lcurl

# Compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean up object files and the binary
clean:
	rm -f $(OBJ_FILES) $(TARGET)

