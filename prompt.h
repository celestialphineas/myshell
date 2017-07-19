// Prompt header file
// ==================
// Author: Celestial Phineas @ ZJU
//         (Yehang YIN)
// This file handles the prompt of the shell
// Unlike POSIX standard, PS1 and PS2 (prompt statements) cannot be editted by
// users.
// The prompt of my shell is a bit different from Bourne Again SHell (bash)
// By default, the prompt line is:
// ---------------------
// username@hostname:pwd
// myshell>
// ---------------------
#ifndef CELPHI_PROMPT_H
#define CELPHI_PROMPT_H
#include "global.h"

// Get the pwd, i.e. the present working directory
// char *get_pwd();
// Get pwd with the user directory replaced by ~
// Note that this creates a new string and please
// do not forget to free the space manually
char *get_pwd_replaced();

// Refresh and get the strings PS1 and PS2
// The data PS1 and PS2 are set to be private
// They are static global variables of prompt.c
// And only with the two functions below,
// the two pointers can be accessed
// char *get_ps1();
// char *get_ps2();

// Print the strings
// Use "write STDOUT_FILENO"
void print_ps1();
void print_ps2();

#endif