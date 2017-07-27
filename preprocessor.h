// Command line string preprocessor
// ================================
#ifndef CELPHI_PREPROCESSOR_H
#define CELPHI_PREPROCESSOR_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef MAX_COMMAND_LEN
#define MAX_COMMAND_LEN 16384
#endif
#ifndef MAX_HOSTNAME_LEN
#define MAX_HOSTNAME_LEN 64
#endif
#ifndef MAX_COMMAND_LINES
#define MAX_COMMAND_LINES 16384
#endif
#ifndef true
#define true    (1)
#endif
#ifndef false
#define false   (0)
#endif

extern char BLANK_CHAR[];

int is_blank_char(char c);
int is_complete_command(char *cmd_str);
// Main targets of the preprocess
// Remove all comments
// Regularize the blank space
// Handle the line-continuation character
// Seperate commands

// This function allocate new space for its result
// If the input is NULL
// The result is a valid empty string
char *remove_comments(char *input);
// And this function removes the extra blank spaces
// Also removes the extra newlines
char *remove_extra_blank(char *input);
// This seperate the input string to commands
// Behavior:
//      Always allocate space for cmdv
//      It is sure that the last element of cmdv is NULL
//      If cmdc is NULL, allocate space
// The return value is cmdv, i.e. an array of pointers
// to strings where the seperated commands are stored
char **seperate_commands(char *input, int *cmdc);
#endif