# Compiler
CC = gcc
# Compiling options
OPTIONS = -c
# Name of the executable
EXECUTABLE = myshell
# Object files
OBJECTS = main.o global.o prompt.o read_input.o
# Source files
SOURCES	= main.c global.c prompt.c read_input.c

debug: OPTIONS += -Wall -g
debug: all

release: OPTIONS += -O3
release: all

all: $(OBJECTS)
	$(CC) -o $(EXECUTABLE) $(OBJECTS)

# make clean
.PHONY: clean
clean:
	rm -f $(OBJECTS) $(EXECUTABLE) *.gch

# All files in this project heavily rely on global.h
%.o: %.c global.h
	$(CC) $(OPTIONS) $<

# Dependencies
main.o:	global.o read_input.o read_input.h
prompt.o: prompt.h
read_input.o: global.o read_input.h prompt.h