// Documentation display
// =====================
// Author: Celestial Phineas @ ZJU
//         (Yehang YIN)
// Simply display docs to stdout
#ifndef CELPHI_DOC_DISPLAY
#define CELPHI_DOC_DISPLAY
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
#endif