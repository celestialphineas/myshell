// Implementation of prompt line
#include "prompt.h"

static char *PS1;
static char *PS2;

char *get_pwd()
{
    // For more info about this system function,
    // read "man getenv"
    char *result = getenv("PWD");
    if(!result) exit(ENVIRONMENT_FAULT_);
    else return result;
}

char *get_pwd_replaced()
{
    int i, j;
    char *home = getenv("HOME");
    char *pwd = get_pwd();
    // Allocate spce to hold the replaced sequence
    char *replaced = (char*)malloc(MAX_PATH_LEN * sizeof(char));
    if(!replaced) exit(MEM_ALLOC_ERR_);
    strcpy(replaced, pwd);
    // Replacement
    for(i = 0; i < strlen(home); i++)
    {
        // If the two sequeces cannot match, return
        if(pwd[i] != home[i]) return replaced;
    }
    replaced[0] = '~';
    for(i = strlen(home), j = 1; i < strlen(pwd); i++, j++)
    {
        replaced[j] = pwd[i];
    }
    // End of the string
    replaced[j] = 0;
    return replaced;
}

char *get_user()
{
    char *result = getenv("USER");
    if(!result) exit(ENVIRONMENT_FAULT_);
    else return result;
}

char *get_ps1()
{
    char *user, *pwd;
    // Allocate space if ps1 has not been initialized
    if(!PS1) PS1 = (char*)malloc(MAX_PROMPT_LEN * sizeof(char));
    if(!PS1) exit(MEM_ALLOC_ERR_);
    // HOSTNAME external char* defined in global.c
    // Declaration of this variable is in global.h
    // Prototype: extern char* HOSTNAME;
    // This should be initialized in the init function
    if(!HOSTNAME) exit(INIT_ERR_);
    // Get the environment variables
    user = get_user();
    pwd = get_pwd_replaced();
    // Write PS1
    sprintf(PS1,
        USER_COLOR "%s" HOSTNAME_COLOR "@%s"
        DEFAULT_COLOR ":" PWD_COLOR "%s"
        DEFAULT_COLOR "\nmyshell> ",
        user, HOSTNAME, pwd);
    return PS1;
}

char *get_ps2()
{
    // Allocate space if ps2 has not been initialized
    if(!PS2)
    {
        PS2 = (char*)malloc(3 * sizeof(char));
        PS2[0] = '>'; PS2[1] = ' '; PS2[2] = 0;
    }
    if(!PS2) exit(MEM_ALLOC_ERR_);
    return PS2;
}

void print_ps1()
{
    char *ps1 = get_ps1();
    write(STDOUT_FILENO, ps1, strlen(ps1));
}

void print_ps2()
{
    char *ps2 = get_ps2();
    write(STDOUT_FILENO, ps2, strlen(ps2));
}