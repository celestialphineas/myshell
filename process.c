// Implementation of process launch functions
#include "process.h"

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
                fd = open(out_file, O_WRONLY|O_CREAT|O_APPEND);
            else
                fd = open(out_file, O_WRONLY|O_CREAT);
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
                fd = open(err_file, O_WRONLY|O_CREAT|O_APPEND);
            else
                fd = open(err_file, O_WRONLY|O_CREAT);
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

    return new_process;
}

// To do: Handle built-ins
int launch_process_pipeline(ProcessPipeline pl, boolean foreground)
{
    Process *p;
    pid_t forked;
    pid_t group_id;
    if(pl->pgid != 0) group_id = pl->pgid;
    // Traverse all the process in the process pipeline
    for(p = pl; p; p = p->next)
    {
        // Handle the completed and stopped jobs
        if(
            p->process_state == COMPLETED
                && (
                    !(p->pipeline_discipline == NEXT_IF_SUCCUSS && p->status_value != 0)
                    || !(p->pipeline_discipline == NEXT_IF_FAILURE && p->status_value == 0)
                )
        )
            continue;
        else if(p->process_state == STOPPED)
        {
            pid_t temp_pid;
            // Resume a hanging process
            kill(p->pid, SIGCONT);
            // Wait
            temp_pid = waitpid(forked, &(p->status_value), WUNTRACED);
            if(temp_pid == -1) // That is to say the process is stopped
            {
                p->process_state = STOPPED;
                return STOPPED;
            }
            else
                p->process_state = COMPLETED;
        }
        else
        {
            // Handle pipe, create a pipe file
            // Yeah, right, the pipe file is created at the launch time.
            if(p->is_pipe && p->next)
            {
                // I decided to call it little_pipe, because I feel a bit boring.
                // In fact, I had thought of some even more crazy names.
                // Like moe_pipe, or simply haha as the name of the pipe.
                // Hahahahahahahahahahahahahahahahahaha!
                // Damn it! The naming matter!
                // Blablablablabla E-Ah-E-Ah-O
                int little_pipe[2];
                pipe(little_pipe);
                p->fd_stdout = little_pipe[1];
                p->fd_stderr = little_pipe[1];
                p->next->fd_stdin = little_pipe[0];
            }
            forked = fork();
            // In the child process
            if(forked == 0)
            {
                // Handle the pid and pgid first
                if(p == pl) // Process group leader
                    group_id = getpid();
                setpgid(getpid(), group_id);
                // Gain control of the terminal and signals
                if(INTERACTIVE_MODE)
                {
                    if(foreground)
                        tcsetpgrp(MYSHELL_TERM_IN, group_id);
                    // Signals
                    signal(SIGINT, SIG_DFL);
                    signal(SIGQUIT, SIG_DFL);
                    signal(SIGTSTP, SIG_DFL);
                    signal(SIGTTIN, SIG_DFL);
                    signal(SIGTTOU, SIG_DFL);
                    signal(SIGCHLD, SIG_DFL);
                }
                // Set the standard input and output for the new process
                dup2(p->fd_stdin, STDIN_FILENO);
                dup2(p->fd_stdout, STDOUT_FILENO);
                dup2(p->fd_stderr, STDERR_FILENO);
                // TODO: Add the built-in support here
                execvp(p->argv[0], p->argv);
                // If failed
                {
                    char buffer[MAX_COMMAND_LEN] = {};
                    strcat(buffer, "Cannot find executable ");
                    strcat(buffer, p->argv[0]);
                }
                puts("failed");
                // Make sure that the forked process will finish
                exit(1);
            }
            else if(forked > 0) // In the parent process
            {
                // Handle the pid and pgid first
                if(p == pl) // Process group leader
                    group_id = forked;
                p->pid = forked;
                p->pgid = group_id;
                p->process_state = RUNNING;
                // Wait until the child process is done
                {
                    pid_t temp_pid;
                    // wait(&(p->status_value));
                    temp_pid = waitpid(forked, &(p->status_value), WNOHANG);
                    if(temp_pid == -1) // That is to say the process is stopped
                    {
                        p->process_state = STOPPED;
                        return STOPPED;
                    }
                    else
                        p->process_state = COMPLETED;
                }
            }
            else exit(PROCESS_CREATE_ERR_);
        }
        // Put the shell back to the foreground
        tcsetpgrp(MYSHELL_TERM_IN, MYSHELL_PID);
        // Test whether run the next command or not
        if(p->pipeline_discipline == NEXT_IF_SUCCUSS && p->status_value != 0)
            break;
        if(p->pipeline_discipline == NEXT_IF_FAILURE && p->status_value == 0)
            break;
    }
    return COMPLETED;
}