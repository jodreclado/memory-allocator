# Makefile for the memory management package.
# CSE 374, HW 6, Team 'bf'
# Authors: Manchen Jin, Rogers Xiang
# May 24, 2018
# Adapted from a class example makefile (MakefileV2_RenameToMakefile).

# The compiler: gcc for C programs
CC = gcc

# Compiler flags:
# -Wall for debugger warnings
# -std=c11 for updated standards
# -g for debugging info
CFLAGS = -Wall -std=c11 -g

# The name of the program to be produced.
TARGET = bench

# A "phony" target that tells make what other targets to build.
all: $(TARGET)

# All the .o files we need for our executable.
OBJS = bench.o getmem.o freemem.o get_mem_stats.o print_heap.o mem_utils.o

# The executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) -lm

# Individual source files
getmem.o: getmem.c mem.h mem_impl.h
	$(CC) $(CFLAGS) -c getmem.c

freemem.o: freemem.c mem.h mem_impl.h
	$(CC) $(CFLAGS) -c freemem.c

get_mem_stats.o: get_mem_stats.c mem.h mem_impl.h
	$(CC) $(CFLAGS) -c get_mem_stats.c

print_heap.o: print_heap.c mem.h mem_impl.h
	$(CC) $(CFLAGS) -c print_heap.c

mem_utils.o: mem_utils.c mem.h mem_impl.h
	$(CC) $(CFLAGS) -c mem_utils.c

bench.o: bench.c mem.h
	$(CC) $(CFLAGS) -c bench.c

# A "phony" target to run the bench test program with default parameters. 
test: $(TARGET)
	./$(TARGET)

# A "phony" target to remove built files and backups
clean:
	rm -f *.o $(TARGET) *~ print_heap.txt
