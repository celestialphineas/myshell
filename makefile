# Compiler
CC = gcc
# Compiling options
OPTIONS = -c
# Name of the executable
EXECUTABLE = myshell
# Name of the test
TESTEXEC = test
# Object files
OBJECTS = global.o prompt.o read_input.o message.o var_table.o\
hash_map.o job_control.o
# Main object
MAINOBJ = main.o
# Test main object
TESTMAINOBJ = main.test.o
# Source files
SOURCES	= main.c main.test.c global.c prompt.c read_input.c message.c\
var_table.c hash_map.c job_control.c
# Headers
HEADERS = global.h prompt.h read_input.h message.h var_table.h\
hash_map.h job_control.h

debug: OPTIONS += -Wall -g
debug: all

release: OPTIONS += -O3
release: all

test: OPTIONS += -Wall -g
test: $(OBJECTS) $(TESTMAINOBJ)
	$(CC) -o $(TESTEXEC) $(TESTMAINOBJ) $(OBJECTS)

all: $(OBJECTS) $(MAINOBJ)
	$(CC) -o $(EXECUTABLE) $(MAINOBJ) $(OBJECTS)

# make clean
.PHONY: clean
clean:
	rm -f $(OBJECTS) $(MAINOBJ) $(TESTMAINOBJ) $(EXECUTABLE) *.gch

# All files in this project heavily rely on global.h
%.o: %.c global.h
	$(CC) $(OPTIONS) $<

# Dependencies
main.o:	$(OBJECTS) $(HEADERS)
	$(CC) $(OPTIONS) main.c
main.test.o: main.test.c $(OBJECTS) $(HEADERS)
	$(CC) $(OPTIONS) main.test.c
prompt.o: prompt.h
read_input.o: global.o read_input.h prompt.h
message.o: message.h
var_table.o: var_table.h hash_map.o
hash_map.o: hash_map.h
job_control.o: message.o message.h