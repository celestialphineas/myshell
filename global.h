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
#include <signal.h>
#include <termios.h>
#include <sys/wait.h>
#include "var_table.h"
#include "preprocessor.h"

// Boolean definition
// ==================
typedef int boolean;
#define true    (1)
#define false   (0)

// Exit codes
// ==========
// The exit codes definitions end with an extra underscore
#define SUCCESS_                0
#define MYSHELL_CALL_ERR_       2
#define MEM_ALLOC_ERR_          3
#define INIT_ERR_               4
#define PID_FAILURE_            5
#define HOSTNAME_NOT_FOUND_     6
#define ENVIRONMENT_FAULT_      7
#define VAR_HASH_TABLE_ERR_     8
#define PROCESS_CREATE_ERR_     9

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
#define MAX_COMMAND_LINES   16384
#define MAX_HOSTNAME_LEN    64

// Hostname path
#define HOSTNAME_PATH   "/etc/hostname"

// Global properties
// =================
// myshell pid
extern pid_t MYSHELL_PID;
// myshell terminal file descriptors
extern int MYSHELL_TERM_IN;     // stdin
extern int MYSHELL_TERM_OUT;    // stdout
extern int MYSHELL_TERM_ERR;    // stderr
// myshell path, i.e. initial pwd
extern char *MYSHELL_PATH;
// Hostname of the machine, initialized when the shell is started
extern char *HOSTNAME;
// Terminal attributes
extern struct termios TERM_ATTR;
// Global arguments
extern int GLOBAL_ARGC;
extern char **GLOBAL_ARGV;
extern int LATEST_STATUS;
// MYSHELL_ARG_OFFSET defines the argument offset
// Negative indicates that there are no arguments passed to the session
// Otherwise, GLOBAL_ARGV[MYSHELL_ARG_OFFSET] and the arguments followed by
// this are passed as arguments $0 ~ ${n}
// Similar to the POSIX standard, $0 is either the bash path (interactive mode)
// or the script path (script mode)
extern int MYSHELL_ARG_OFFSET;
// Blank space characters, defined in preprocessor.c
extern char BLANK_CHAR[];
// Control characters, defined in tokenizer.c
extern char CONTROL_CHAR[];

// myshell flags
// =============
extern boolean INTERACTIVE_MODE;

// Functions
// =========
// Initialization
// --------------
void init();
// This function is used in initialization.
// It can also be used to update the variable table
// When a shift command is called
void push_arg_to_var_table();
// Get shell variables
// -------------------
// Implemented in global.c
char *get_hostname();           // Get the hostname
char *get_myshell_path();       // Get myshell path
// Implemented in prompt.c
char *get_ps1();                // Get PS1
char *get_ps2();                // Get PS2
char *get_pwd();                // Get PWD

void sigchld_handler(int signum);

#endif