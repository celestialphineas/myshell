// Command line string preprocessor
// ================================
#ifndef CELPHI_PREPROCESSOR_H
#define CELPHI_PREPROCESSOR_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef MAX_COMMAND_LEN
#define MAX_COMMAND_LEN 16384
#endif

int is_complete_command(char *cmd_str);

#endif