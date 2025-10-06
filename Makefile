# Makefile for COMP20003 Assignment 2

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g

# Common source files used by both executables
COMMON_SRCS = data.c list.c bit.c

# Executable names
EXEC1 = dict1
EXEC2 = dict2

# Object files for each executable
OBJS1 = main.o $(COMMON_SRCS:.c=.o)
OBJS2 = dict2.o patricia.o $(COMMON_SRCS:.c=.o)

# Default target: build both executables
all: $(EXEC1) $(EXEC2)

# Rule to build the Stage 1 executable (dict1)
$(EXEC1): $(OBJS1)
	$(CC) $(CFLAGS) -o $(EXEC1) $(OBJS1)

# Rule to build the Stage 2 executable (dict2)
$(EXEC2): $(OBJS2)
	$(CC) $(CFLAGS) -o $(EXEC2) $(OBJS2)

# Specific rule for dict2's main object file to avoid conflicts
dict2.o: dict2.c patricia.h data.h list.h
	$(CC) $(CFLAGS) -c dict2.c -o dict2.o

# Specific rule for the patricia tree object file
patricia.o: patricia.c patricia.h list.h
	$(CC) $(CFLAGS) -c patricia.c -o patricia.o

# Generic rule to compile .c files into .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f *.o $(EXEC1) $(EXEC2)

.PHONY: all clean

