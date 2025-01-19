# Makefile for GTK-based YouTube Downloader

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g `pkg-config --cflags gtk+-3.0` -Wno-unused-parameter
LDFLAGS = `pkg-config --libs gtk+-3.0`

# Target executable
TARGET = youtube_downloader

# Source files
SRCS = youtube_downloader.c

# Build and output directories
BUILD_DIR = build
BIN_DIR = bin

# Object files (place .o files in build directory)
OBJS = $(BUILD_DIR)/$(SRCS:.c=.o)

# Default target
all: $(BUILD_DIR) $(BIN_DIR) $(BIN_DIR)/$(TARGET)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Create bin directory
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Build target executable
$(BIN_DIR)/$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile source files into object files
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Install the binary to /usr/local/bin
install: $(BIN_DIR)/$(TARGET)
	sudo install -m 755 $(BIN_DIR)/$(TARGET) /usr/local/bin/

# Clean up generated files
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Phony targets
.PHONY: all clean install
