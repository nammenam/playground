# Define compiler
CC=g++

# Compiler flags
CFLAGS=-I../firmware/src/kalmanfilter -std=c++11 -g

# Source files
SOURCES=../firmware/src/kalmanfilter/KalmanFilter.cpp \
        ../firmware/src/utility/utility.cpp \
        kalman_unit_test.cpp

# Header files
HEADERS=../firmware/src/kalmanfilter/KalmanFilter.hpp \
        ../firmware/src/utility/utility.hpp \
        ../firmware/src/datastructures.h

# Output binary
OUTPUT=bin/kftest

# Default target
all: prebuild $(OUTPUT)

prebuild:
	@mkdir -p bin

$(OUTPUT): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(SOURCES) -o $(OUTPUT)

# Clean target
clean:
	@rm -f $(OUTPUT)
