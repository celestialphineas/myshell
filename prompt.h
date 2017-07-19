// Prompt header file
#ifndef CELPHI_PROMPT_H
#define CELPHI_PROMPT_H
#include "global.h"

// Get the pwd, i.e. the present working directory
char *get_pwd();
// Get pwd with the user directory replaced by ~
// Note that this creates a new string and please
// do not forget to free the space manually
char *get_pwd_replaced();

// Refresh and get the strings PS1 and PS2
char *get_ps1();
char *get_ps2();

// Print the strings
void print_ps1();
void print_ps2();

#endif