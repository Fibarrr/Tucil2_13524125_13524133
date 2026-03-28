CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -pedantic -Isrc
TARGET = bin/main_parser_test
SOURCE = src/main.cpp
OUTPUT_DIR = bin

# Parameter yang bisa disesuaikan
#ini ubah ubah
# FILE ?= test/cow.obj
# DEPTH ?= 7

# Default target
all: build

build: $(TARGET)

$(TARGET): $(SOURCE)
	@mkdir -p $(OUTPUT_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@
	@echo "✓ Build berhasil: $(TARGET)"

# Run dengan input interaktif
run: build
	$(TARGET)

# Run dengan parameter otomatis
run-auto: build
	$(TARGET) $(FILE) $(DEPTH)

clean:
	rm -f $(TARGET)
	rm -f test/*-voxelized.obj
	@echo "✓ Cleanup selesai"

.PHONY: all build run run-auto clean
