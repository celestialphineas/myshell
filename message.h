// Documentation display
// =====================
// Author: Celestial Phineas @ ZJU
//         (Yehang YIN)
// Simply display docs to stdout
#ifndef CELPHI_MESSAGE_H
#define CELPHI_MESSAGE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#ifndef MAX_PATH_LEN
#define MAX_PATH_LEN    4096
#endif
#define DOC_PATH  "doc/"  
void print_docs(const char *filename);
void print_myshell_err(const char *message);
#endif