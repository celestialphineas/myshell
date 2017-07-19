// Read input header file
// ======================
// Author: Celestial Phineas @ ZJU
// This file contains prototypes of functions that read the command line
// from stdin
#ifndef CELPHI_READ_INPUT_H
#define CELPHI_READ_INPUT_H
#include "global.h"
#include "prompt.h"

// Note that the following get-command functions:
// 1. Allocate memory, don't forget to free
// 2. Print the prompts

// Max ccommand line length definition see global.h
// #define MAX_COMMAND_LEN     16384

// This function gets input from stdin in a blocking manner
// With the prompt of PS1
char *get_command();
// And this function gets continued input
// With the prompt of PS2
char *get_continued_command();
// Give the status of a command line
int command_status(char *command);

#endif