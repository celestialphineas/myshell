#include "message.h"
#include "wait.h"

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
        int status;
        pid_t forked = fork();
        if(!forked) execl("/bin/cat", "cat", path, NULL);
        else if(forked == -1) exit(1);
        else wait(&status);
    }
    puts("");
    return;
}

void print_myshell_err(const char *message)
{
    write(2, "myshell: ", strlen("myshell: "));
    write(2, message, strlen(message));
    write(2, "\n", strlen("\n"));
    return;
}