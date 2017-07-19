# Compiler
CC = gcc
# Compiling options
OPTIONS = -c
# Name of the executable
EXECUTABLE = myshell
# Object files
OBJECTS = main.o
# Source files
SOURCES	= main.c

debug: OPTIONS += -Wall -g
debug: all

release: OPTIONS += -O3
release: all

all: $(OBJECTS)
	$(CC) -o $(EXECUTABLE) $(OBJECTS)

.PHONY: clean
clean:
	rm -f $(OBJECTS) $(EXECUTABLE) *.gch

%.o: %.c
	$(CC) $(OPTIONS) $<

# Dependencies
main.o:	main.h

