// Command line string preprocessor
// ================================
// Author: Celestial Phineas @ ZJU
//         (Yehang YIN)
// Main targets of preprocessing
// Remove all comments
// Regularize the blank space
// Handle the line-continuation character
// separate commands
#ifndef CELPHI_PREPROCESSOR_H
#define CELPHI_PREPROCESSOR_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "message.h"

// See global.h for detailed definitions
#ifndef MEM_ALLOC_ERR_
#define MEM_ALLOC_ERR_          3
#endif
#ifndef BUILT_IN_RUNTIME_ERR_
#define BUILT_IN_RUNTIME_ERR_   11
#endif
#ifndef MAX_COMMAND_LEN
#define MAX_COMMAND_LEN         16384
#endif
#ifndef MAX_HOSTNAME_LEN
#define MAX_HOSTNAME_LEN        64
#endif
#ifndef MAX_COMMAND_LINES
#define MAX_COMMAND_LINES       16384
#endif
#ifndef MAX_TOKEN_NUM
#define MAX_TOKEN_NUM           1024
#endif
#ifndef true
#define true    (1)
#endif
#ifndef false
#define false   (0)
#endif

// This exports the blank char sequence,
// characters in which are seen as the blank space characters.
// Space and tab by default.
extern char BLANK_CHAR[];

// Do what it is called! How obvious!
int is_blank_char(char c);
int is_complete_command(char *cmd_str);

// This function allocate new space for its result
// If the input is NULL
// The result is a valid empty string
char *remove_comments(char *input);
// And this function removes the extra blank spaces
// Also removes the extra newlines
char *remove_extra_blank(char *input);
// This separate the input string to commands
// Behavior:
//      Always allocate space for cmdv if no error occurs
//      It is sure that the last element of cmdv is NULL
//      If cmdc is NULL, allocate space
//      Set cmdc -1, if an error occurs, and returns NULL
// The return value is cmdv, i.e. an array of pointers
// to strings where the separated commands are stored
char **separate_commands(char *input, int *cmdc);
#endif