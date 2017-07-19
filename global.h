// Global definations
#ifndef CELPHI_GLOBAL_H
#define CELPHI_GLOBAL_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

// Exit code
#define SUCCESS_                0
#define MEM_ALLOC_ERR_          2
#define INIT_ERR_               3
#define HOSTNAME_NOT_FOUND_     5
#define ENVIRONMENT_FAULT_      6

// Initialization
void init();

// Prompt style
// Myshell specification: the prompt line is built-in
// POSIX standard for PS1 and PS2 is not supported.
// That is to say a user can never change the format
// of the prompt line.
// ==============================
// USER_COLOR       Color of the username,
//                  green by default
// HOSTNAME_COLOR   Color of the hostname,
//                  green by default
// PWD_COLOR        Color of the pwd,
//                  blue by default
// DEFAULT_COLOR    Default escape
#define USER_COLOR      "\033[32;1m"
#define HOSTNAME_COLOR  "\033[32;1m"
#define PWD_COLOR       "\033[34;1m"
#define DEFAULT_COLOR   "\033[0m"

#define HOSTNAME_PATH   "/etc/hostname"
// Max lengths of some variables
#define MAX_PATH_LEN        4096
#define MAX_PROMPT_LEN      16384
#define MAX_HOSTNAME_LEN    64

// Hostname of the machine, initialized when the shell is started
extern char *HOSTNAME;

// Initialize the hostname
char *get_hostname();

#endif