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
char *get_ps1();
char *get_ps2();

#endif