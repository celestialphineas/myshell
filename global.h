// Global definitions
// ==================
// Author: Celestial Phineas @ ZJU
//         (Yehang YIN)
// This file contains the very foundamental and global variables of myshell
#ifndef CELPHI_GLOBAL_H
#define CELPHI_GLOBAL_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

// Boolean definition
// ==================
typedef int boolean;
#define true    (1)
#define false   (0)

// Exit codes
// ==========
// The exit codes definitions end with an extra underscore
#define SUCCESS_                0
#define MEM_ALLOC_ERR_          2
#define INIT_ERR_               3
#define HOSTNAME_NOT_FOUND_     5
#define ENVIRONMENT_FAULT_      6

// Prompt style
// ============
// Myshell specification: the prompt line is built-in
// POSIX standard for PS1 and PS2 is not supported.
// That is to say a user can never change the format
// of the prompt line.
// USER_COLOR       Color of the username,
//                  green by default
// HOSTNAME_COLOR   Color of the hostname,
//                  green by default
// PWD_COLOR        Color of the pwd,
//                  blue by default
// PROMT_COLOR      color of "myshell> "
//                  purple by default
// DEFAULT_COLOR    Default escape
#define USER_COLOR      "\033[32;1m"
#define HOSTNAME_COLOR  "\033[32;1m"
#define PWD_COLOR       "\033[34;1m"
#define PROMPT_COLOR    "\033[35;1m"
#define DEFAULT_COLOR   "\033[0m"
#define PROMPT_STRING   "myshell> "

// Limit macros
// ============
// Max lengths of some variables
#define MAX_PATH_LEN        4096
#define MAX_PROMPT_LEN      16384
#define MAX_COMMAND_LEN     16384
#define MAX_HOSTNAME_LEN    64

// Hostname path
#define HOSTNAME_PATH   "/etc/hostname"
// Hostname of the machine, initialized when the shell is started
extern char *HOSTNAME;
// myshell path, i.e. initial pwd
extern char *MYSHELL_PATH;

// Functions
// =========
// Initialization
// --------------
void init();
// Get shell variables
// -------------------
// Implemented in global.c
char *get_hostname();           // Get the hostname
char *get_myshell_path();       // Get myshell path
// Implemented in prompt.c
char *get_ps1();                // Get PS1
char *get_ps2();                // Get PS2
char *get_pwd();                // Get PWD

#endif