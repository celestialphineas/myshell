// Implemetation of the command line input fcuntions
#include "read_input.h"

char *get_command()
{
    char *result = NULL;
    size_t command_length = 0;
    print_ps1();
    // View more info of getline function in man getline(3)
    getline(&result, &command_length, stdin);
    return result;
}

char *get_continued_command()
{
    char *result = NULL;
    size_t command_length = 0;
    print_ps2();
    getline(&result, &command_length, stdin);
    return result;
}
