#include "doc_display.h"

void print_docs(const char *filename)
{
    char path[MAX_PATH_LEN];
    path[0] = 0;
    strcat(path, DOC_PATH);
    strcat(path, filename);
    if(access(DOC_PATH, R_OK))
    {
        puts("Cannot find the documentation path.");
        puts("Please check if myshell is correctly installed.");
    }
    if(access(path, R_OK))
    {
        printf("Undocumented \"%s\"\n", filename);
    }
    else
    {
        execl("/bin/cat", "cat", path, NULL);
    }
    return;
}