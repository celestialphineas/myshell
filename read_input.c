// Implemetation of the command line input fcuntions
#include "read_input.h"

char *prompt1_read()
{
    char *result = NULL;
    char buffer[MAX_COMMAND_LEN] = {};
    int i = 0;
    int char_read;
    print_ps1();
    while((char_read = getchar()) != '\n')
    {
        if(char_read == EOF)
        {
            printf("exit\n");
            exit(0);
        }
        else buffer[i++] = (char)char_read;
    }
    buffer[i++] = '\n';
    buffer[i] = 0;
    result = (char*)malloc((i + 1) * sizeof(char));
    strcpy(result, buffer);
    return result;
}

char *prompt2_read()
{
    char *result = NULL;
    char buffer[MAX_COMMAND_LEN] = {};
    int i = 0;
    int char_read;
    print_ps2();
    while((char_read = getchar()) != '\n')
    {
        if(char_read == EOF)
        {
            printf("exit\n");
            exit(0);
        }
        else buffer[i++] = (char)char_read;
    }
    buffer[i++] = '\n';
    buffer[i] = 0;
    result = (char*)malloc((i + 1) * sizeof(char));
    strcpy(result, buffer);
    return result;
}
