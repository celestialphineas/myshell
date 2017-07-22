# Compiler
CC = gcc
# Compiling options
OPTIONS = -c
# Name of the executable
EXECUTABLE = myshell
# Object files
OBJECTS = main.o global.o prompt.o read_input.o doc_display.o var_table.o\
hash_map.o
# Source files
SOURCES	= main.c global.c prompt.c read_input.c doc_display.c var_table.c\
hash_map.c
# Headers
HEADERS = global.h prompt.h read_input.h doc_display.h var_table.h\
hash_map.h

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
main.o:	global.o read_input.o read_input.h doc_display.o doc_display.h
prompt.o: prompt.h
read_input.o: global.o read_input.h prompt.h
doc_display.o: doc_display.h
var_table.o: var_table.h hash_map.o
hash_map.o: hash_map.h