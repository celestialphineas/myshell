#include "global.h"

// Concrete global properties
pid_t MYSHELL_PID;
int MYSHELL_TERM_IN;
int MYSHELL_TERM_OUT;
int MYSHELL_TERM_ERR;
char *MYSHELL_PATH;
char *HOSTNAME;
boolean INTERACTIVE_MODE;
struct termios TERM_ATTR;
int GLOBAL_ARGC;
char **GLOBAL_ARGV;
int MYSHELL_ARG_OFFSET;
int LATEST_STATUS;

extern char** environ;

// Static prototypes
static void handle_myshell_signals();
static char *create_hostname();
static void grab_term_ctrl();

void init()
{
    // Initialize terminal file descriptors
    MYSHELL_TERM_IN = STDIN_FILENO;
    MYSHELL_TERM_OUT = STDOUT_FILENO;
    MYSHELL_TERM_ERR = STDERR_FILENO;

    // If the standard input of the program is not a tty,
    // then assume that the program must not be running in interactive mode
    if(!isatty(MYSHELL_TERM_IN)) INTERACTIVE_MODE = false;

    // Handle myshell signals
    handle_myshell_signals();

    // Initialize myshell pid
    if(INTERACTIVE_MODE)
        MYSHELL_PID = getpid();
    // If failed to get the pid
    if(MYSHELL_PID < 0)  exit(PID_FAILURE_);
    // Set process group ID
    // Make myshell the process group leader
    setpgid(MYSHELL_PID, MYSHELL_PID);

    // Initialize myshell path
    // First get the initial pwd
    char *init_pwd = get_pwd();
    // Then copy the string
    MYSHELL_PATH = (char*)malloc(strlen(init_pwd) * sizeof(char));
    // If fail to malloc, exit with error
    if(!MYSHELL_PATH) exit(MEM_ALLOC_ERR_);
    // String copy
    strcpy(MYSHELL_PATH, init_pwd);

    // Get the hostname
    HOSTNAME = create_hostname();
    
    // Grab control of the terminal
    grab_term_ctrl();

    // Push the environment variables to the variable table
    init_var_table();
    push_environ_to_var_table();
    // Push arguments to the variable table
    push_arg_to_var_table();
    return;
}

static void handle_myshell_signals()
{
    // Take over the signals if myshell is running in interactive mode
    if(INTERACTIVE_MODE)
    {
        // Key board interrupt (^C)
        signal(SIGINT, SIG_IGN);
        // Quit (^Q)
        signal(SIGQUIT, SIG_IGN);
        // Terminal stop (^Z)
        signal(SIGTSTP, SIG_IGN);
        // Read from terminal
        signal(SIGTTIN, SIG_IGN);
        // Write to terminal
        signal(SIGTTOU, SIG_IGN);
        // Child process signal handler
        signal(SIGCHLD, sigchld_handler);
    }
}

static char *create_hostname()
{
    // Hostname string
    char *hostname = (char*)malloc(MAX_HOSTNAME_LEN * sizeof(char));
    if(!hostname) exit(MEM_ALLOC_ERR_);
    // File discriptor of the opened hostname file
    int fd_hostname = open(HOSTNAME_PATH, O_RDONLY);
    // Exit if fails
    if(fd_hostname == -1) exit(HOSTNAME_NOT_FOUND_);
    // Read in the hostname
    read(fd_hostname, (void*)hostname, MAX_HOSTNAME_LEN);
    // Change the line feed to '\0'
    hostname[strlen(hostname) - 1] = 0;
    // Return the hostname
    return hostname;
}

static void grab_term_ctrl()
{
    // Take over the terminal if myshell is running in interactive mode
    if(INTERACTIVE_MODE)
    {
        // Set the forground process group ID
        tcsetpgrp(MYSHELL_TERM_IN, MYSHELL_PID);
        // Get the state of FD and put it in TERM_ATTR
        tcgetattr(MYSHELL_TERM_IN, &TERM_ATTR);
    }
}

void push_environ_to_var_table()
{
    char key_buffer[32];
    char **p = environ;
    while(*p)
    {
        int equal_index = 0;
        int rest;
        Variable var;
        var.elec = 1;
        var.elev = (char**)malloc(sizeof(char*));
        if(!var.elev) exit(MEM_ALLOC_ERR_);
        while((*p)[equal_index] != '=')
        {
            if(!(*p)[equal_index]) break;
            equal_index++;
        }
        memcpy(key_buffer, *p, equal_index);
        key_buffer[equal_index] = 0;
        rest = strlen(*p) - equal_index;
        *(var.elev) = (char*)malloc(rest * sizeof(char));
        if(!*var.elev) exit(MEM_ALLOC_ERR_);
        memcpy(*var.elev, *p + equal_index + 1, rest);
        (*var.elev)[rest] = 0;
        update_variable(key_buffer, &var);
        p++;
    }
    return;
}

void push_arg_to_var_table()
{
    int i = 0;
    char key_buffer[4];
    char **p = GLOBAL_ARGV + MYSHELL_ARG_OFFSET;
    while(p < GLOBAL_ARGV + GLOBAL_ARGC)
    {
        Variable var;
        var.elec = 1;
        var.elev = (char**)malloc(sizeof(char*));
        if(!var.elev) exit(MEM_ALLOC_ERR_);
        *var.elev = (char*)malloc((strlen(*p) + 1) * sizeof(char));
        if(!*var.elev) exit(MEM_ALLOC_ERR_);
        strcpy(*var.elev, *p);
        sprintf(key_buffer, "%d", i++);
        update_variable(key_buffer, &var);
        p++;
    }
    for(i = GLOBAL_ARGC - MYSHELL_ARG_OFFSET; i < GLOBAL_ARGC; i++)
    {
        sprintf(key_buffer, "%d", i);
        delete_variable(key_buffer);
    }
    return;
}

char *get_hostname()
{
    if(!HOSTNAME) HOSTNAME = create_hostname();
    return HOSTNAME;
}

char *get_myshell_path()
{
    if(!MYSHELL_PATH) exit(INIT_ERR_);
    return MYSHELL_PATH;
}