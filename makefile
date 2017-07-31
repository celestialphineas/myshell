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
hash_map.o job_control.o preprocessor.o tokenizer.o expansion.o\
parser.o built_in.o
# Main object
MAINOBJ = main.o
# Test main object
TESTMAINOBJ = main.test.o
# Source files
SOURCES	= main.c global.c prompt.c read_input.c message.c\
var_table.c hash_map.c job_control.c preprocessor.c tokenizer.c expansion.c\
parser.c built_in.c
# Headers
HEADERS = global.h prompt.h read_input.h message.h var_table.h\
hash_map.h job_control.h preprocessor.h tokenizer.h expansion.h\
parser.h built_in.h

release: OPTIONS += -Wall
release: all

debug: OPTIONS += -Wall -g
debug: all

test: OPTIONS += -Wall -g
test: $(OBJECTS) $(TESTMAINOBJ)
	$(CC) -o $(TESTEXEC) $(TESTMAINOBJ) $(OBJECTS)

help: message.h message.o help.c
	$(CC) -o help $(OBJECTS) help.c

all: $(OBJECTS) $(MAINOBJ) help
	$(CC) -o $(EXECUTABLE) $(MAINOBJ) $(OBJECTS)

# make clean
.PHONY: clean
clean:
	rm -f $(OBJECTS) $(MAINOBJ) $(TESTMAINOBJ) $(EXECUTABLE) $(TESTEXEC) *.gch help

# All files in this project heavily rely on global.h
%.o: %.c global.h
	$(CC) $(OPTIONS) $<

# Dependencies
main.o:	$(SOURCES) $(HEADERS)
	$(CC) $(OPTIONS) main.c
main.test.o: main.test.c $(SOURCES) $(HEADERS)
	$(CC) $(OPTIONS) main.test.c
prompt.o: prompt.h
read_input.o: global.c read_input.h prompt.h
message.o: message.h
var_table.o: var_table.h hash_map.c
hash_map.o: hash_map.h
job_control.o: message.c message.h
preprocessor.o: preprocessor.h
tokenizer.o: preprocessor.h preprocessor.c
expansion.o: preprocessor.h tokenizer.h
parser.o: tokenizer.h expansion.h job_control.h
echo.o: built_in.h built_in.c