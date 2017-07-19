#include "global.h"

char *HOSTNAME;
char *MYSHELL_PATH;

// Static prototypes
static char *create_hostname();

void init()
{
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
    
    return;
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