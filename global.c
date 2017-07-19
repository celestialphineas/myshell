#include "global.h"

char *HOSTNAME;

void init()
{
    // Get the hostname
    HOSTNAME = get_hostname();
    return;
}

char *get_hostname()
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
    // Change the line feed to 0
    hostname[strlen(hostname) - 1] = 0;
    // Return the hostname
    return hostname;
}


