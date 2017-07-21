#include "global.h"

// Concrete global properties
int MYSHELL_PID;
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

    return;
}

static void handle_myshell_signals()
{
    // Take over the signals if myshell is running in interactive mode
    if(INTERACTIVE_MODE)
    {
        // Todo: Handle this:
        signal(SIGINT, SIG_IGN);
        // These should not be handled
        signal(SIGQUIT, SIG_IGN);
        signal(SIGTSTP, SIG_IGN);
        signal(SIGTTIN, SIG_IGN);
        signal(SIGTTOU, SIG_IGN);
        signal(SIGCHLD, SIG_IGN);
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
        tcsetpgrp(MYSHELL_TERM_IN, MYSHELL_PID);
        tcgetattr(MYSHELL_TERM_IN, &TERM_ATTR);
    }
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