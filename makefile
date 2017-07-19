# Compiler
CC = gcc
# Compiling options
OPTIONS = -c
# Name of the executable
EXECUTABLE = myshell
# Object files
OBJECTS = main.o global.o prompt.o
# Source files
SOURCES	= main.c global.c prompt.c

debug: OPTIONS += -Wall -g
debug: all

release: OPTIONS += -O3
release: all

all: $(OBJECTS)
	$(CC) -o $(EXECUTABLE) $(OBJECTS)

.PHONY: clean
clean:
	rm -f $(OBJECTS) $(EXECUTABLE) *.gch

%.o: %.c global.h
	$(CC) $(OPTIONS) $<

# Dependencies
main.o:	global.o prompt.o prompt.h
prompt.o: prompt.h