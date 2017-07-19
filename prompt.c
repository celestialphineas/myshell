// Implementation of prompt line
#include "prompt.h"

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
    for(i = strlen(home))
}

char *get_ps1()
{
    return NULL;
}

char *get_ps2()
{
    return NULL;
}