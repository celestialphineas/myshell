# Compiler
CC = gcc
# Compiling options
OPTIONS = -c
# Name of the executable
EXECUTABLE = myshell
# Object files
OBJECTS = main.o global.o prompt.o read_input.o message.o var_table.o\
hash_map.o process.o
# Source files
SOURCES	= main.c global.c prompt.c read_input.c message.c var_table.c\
hash_map.c process.c
# Headers
HEADERS = global.h prompt.h read_input.h message.h var_table.h\
hash_map.h process.h

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
main.o:	global.o read_input.o read_input.h message.o message.h
prompt.o: prompt.h
read_input.o: global.o read_input.h prompt.h
message.o: message.h
var_table.o: var_table.h hash_map.o
hash_map.o: hash_map.h
process.o: message.o message.h