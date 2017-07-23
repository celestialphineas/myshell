// Implementation of process launch functions
#include "process.h"

// To do: Handle built-ins
Process *create_process(
    int argc_, char **argv_,
    boolean is_pipe_, 
    boolean append_out_,
    PipelineDiscipline pipeline_discipline_,
    char *in_file, char *out_file, char *err_file,
    // If the in_file, out_file or err_file is given NULL,
    // then the file descriptors come to work
    // These file discriptors are used as fallbacks
    int in_file_fd, int out_file_fd, int err_file_fd)
{
    int i;
    Process *new_process = (Process*)malloc(sizeof(Process));
    if(!new_process) exit(MEM_ALLOC_ERR_);
    new_process->next = NULL;
    new_process->argc = argc_;
    new_process->argv = NULL;
    new_process->pid = 0;
    new_process->pgid = 0;
    new_process->process_state = UNSTARTED;
    new_process->status_value = 0;
    new_process->is_pipe = is_pipe_;
    new_process->append_out = append_out_;
    new_process->pipeline_discipline = pipeline_discipline_;
    new_process->fd_stdin = 0;
    new_process->fd_stdout = 1;
    new_process->fd_stderr = 2;

    if(in_file)
    {
        // If the file is not read accessible
        if(access(in_file, R_OK) != 0)
        {
            char buffer[100] = {};
            strcat(buffer, in_file);
            strcat(buffer, " is not read accessible.");
            print_myshell_err(buffer);
            free(new_process);
            return NULL;
        }
        else // the file is read accessible
        {
            int fd = open(in_file, O_RDONLY);
            new_process->fd_stdin = fd;
        }
    }
    else // Use the file descriptor
    {
        // If not a valid file descriptor
        if(fcntl(in_file_fd, F_GETFL) < 0)
        {
            char buffer[100] = {};
            sprintf(buffer, "%d", in_file_fd);
            strcat(buffer, " is not a vaild file descriptor.");
            print_myshell_err(buffer);
            free(new_process);
            return NULL;
        }
        else // the file descriptor is valid
        {
            new_process->fd_stdin = in_file_fd;
        }
    }

    if(out_file)
    {
        // If the file is not write accessible
        if(access(out_file, W_OK) != 0)
        {
            char buffer[100] = {};
            strcat(buffer, out_file);
            strcat(buffer, " is not write accessible.");
            print_myshell_err(buffer);
            free(new_process);
            return NULL;
        }
        else // the file is write accessible
        {
            int fd;
            // If appending mode is on
            // Use different open mode to write the file
            if(append_out_)
                fd = open(out_file, O_WRONLY|O_APPEND);
            else
                fd = open(out_file, O_WRONLY);
            new_process->fd_stdout = fd;
        }
    }
    else // Use the file descriptor
    {
        // If not a valid file descriptor
        if(fcntl(out_file_fd, F_GETFL) < 0)
        {
            char buffer[100] = {};
            sprintf(buffer, "%d", out_file_fd);
            strcat(buffer, " is not a valid file descriptor.");
            print_myshell_err(buffer);
            free(new_process);
            return NULL;
        }
        else // the file descriptor is valid
        {
            new_process->fd_stdout = out_file_fd;
        }
    }

    if(err_file)
    {
        // If the file is not write accessible
        if(access(err_file, W_OK) != 0)
        {
            char buffer[100] = {};
            strcat(buffer, err_file);
            strcat(buffer, " is not write accessible.");
            print_myshell_err(buffer);
            free(new_process);
            return NULL;
        }
        else // the file is write accessible
        {
            int fd;
            // If appending mode is on
            // Use different open mode to write the file
            if(append_out_)
                fd = open(err_file, O_WRONLY|O_APPEND);
            else
                fd = open(err_file, O_WRONLY);
            new_process->fd_stderr = fd;
        }
    }
    else // Use the file descriptor
    {
        // If not a valid file descriptor
        if(fcntl(err_file_fd, F_GETFL) < 0)
        {
            char buffer[100] = {};
            sprintf(buffer, "%d", err_file_fd);
            strcat(buffer, " is not a valid file descriptor.");
            print_myshell_err(buffer);
            free(new_process);
            return NULL;
        }
        else // the file descriptor is valid
        {
            new_process->fd_stderr = out_file_fd;
        }
    }

    // Copy argv
    new_process->argv = (char**)malloc(sizeof(char*) * (new_process->argc + 1));
    if(!new_process->argv) exit(MEM_ALLOC_ERR_);
    new_process->argv[new_process->argc] = NULL;
    for(i = 0; i < new_process->argc; i++)
    {
        new_process->argv[i]
            = (char*)malloc((strlen(argv_[i]) + 1) * sizeof(char));
        if(!new_process->argv[i]) exit(MEM_ALLOC_ERR_);
        strcpy(new_process->argv[i], argv_[i]);
    }

    return 0;
}