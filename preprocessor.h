// Command line string preprocessor
// ================================
// Author: Celestial Phineas @ ZJU
//         (Yehang YIN)
// Main targets of preprocessing
// Remove all comments
// Regularize the blank space
// Handle the line-continuation character
// Seperate commands
#ifndef CELPHI_PREPROCESSOR_H
#define CELPHI_PREPROCESSOR_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "message.h"

#ifndef MEM_ALLOC_ERR_
#define MEM_ALLOC_ERR_ 3
#endif
#ifndef MAX_COMMAND_LEN
#define MAX_COMMAND_LEN 16384
#endif
#ifndef MAX_HOSTNAME_LEN
#define MAX_HOSTNAME_LEN 64
#endif
#ifndef MAX_COMMAND_LINES
#define MAX_COMMAND_LINES 16384
#endif
#ifndef MAX_TOKEN_NUM
#define MAX_TOKEN_NUM 1024
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
// This seperate the input string to commands
// Behavior:
//      Always allocate space for cmdv if no error occurs
//      It is sure that the last element of cmdv is NULL
//      If cmdc is NULL, allocate space
//      Set cmdc -1, if an error occurs, and returns NULL
// The return value is cmdv, i.e. an array of pointers
// to strings where the seperated commands are stored
char **seperate_commands(char *input, int *cmdc);
// I use this line to test the preprocessor, lalala $(
// You don\'t need to know whatever these comments are.)
// Lalala and hahah $[
// huhuhuhu]<<>> <;;| &sadf&
// "hjkgjgh" 'hjgjhg' <><>
// As you can see, I here have the Unfinished lines of commands
#endif