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
#endif