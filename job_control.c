#include "job_control.h"

static void restore_signals();

Job *job_list = NULL;
static int global_job_number = 0;

Process *create_process(int argc_, char **argv_,
    boolean is_pipe_, boolean append_out_, boolean append_err_,
    PipelineDiscipline pipeline_discipline_,
    char *in_file, char *out_file, char *err_file,
    int in_file_fd, int out_file_fd, int err_file_fd)
{
    int i;
    Process *new_process = (Process*)malloc(sizeof(Process));
    if(!new_process) exit(MEM_ALLOC_ERR_);
    new_process->next = NULL;
    new_process->argc = argc_;
    new_process->argv = NULL;
    new_process->pid = 0;
    new_process->state = UNSTARTED;
    new_process->status_value = 0;
    new_process->is_pipe = is_pipe_;
    new_process->append_out = append_out_;
    new_process->append_err = append_err_;
    new_process->pipeline_discipline = pipeline_discipline_;
    new_process->fd_stdin = 0;
    new_process->fd_stdout = 1;
    new_process->fd_stderr = 2;

    if(in_file)
    {
        // If the file is not read accessible
        if(access(in_file, R_OK) != 0)
        {
            char buffer[MAX_COMMAND_LEN] = {};
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
    else // NULL, use the file descriptor
    {
        // If not a valid file descriptor
        if(fcntl(in_file_fd, F_GETFL) < 0)
        {
            char buffer[MAX_COMMAND_LEN] = {};
            sprintf(buffer, "%d", in_file_fd);
            strcat(buffer, " is not a valid file descriptor.");
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
            char buffer[MAX_COMMAND_LEN] = {};
            strcat(buffer, out_file);
            strcat(buffer, " is not write accessible.");
            print_myshell_err(buffer);
            free(new_process);
            return NULL;
        }
        else // the file is write accessible
        {
            int fd;
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
            char buffer[MAX_COMMAND_LEN] = {};
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
            char buffer[MAX_COMMAND_LEN] = {};
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
            if(append_err_)
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
            char buffer[MAX_COMMAND_LEN] = {};
            sprintf(buffer, "%d", err_file_fd);
            strcat(buffer, " is not a valid file descriptor.");
            print_myshell_err(buffer);
            free(new_process);
            return NULL;
        }
        else // the file descriptor is valid
        {
            new_process->fd_stderr = err_file_fd;
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

// This function is called in the forked process
void init_child_process(Process *p, pid_t pgid, ForegroundBoolean foreground)
{
    if(!p) return;
    if(p->argc == 0 || !p->argv || !p->argv[0]) return;
    if(INTERACTIVE_MODE)
    {
        // Set pgid
        p->pid = getpid();
        if(!pgid) pgid = p->pid;
        setpgid(p->pid, pgid);
        // If foreground
        // Set the terminal's control process group
        if(foreground)
            tcsetpgrp(MYSHELL_TERM_IN, pgid);
        // Restore the signals
        restore_signals();
    }
    // Set the files
    if(p->fd_stdin != STDIN_FILENO)
    {
        dup2(p->fd_stdin, STDIN_FILENO);
        close(p->fd_stdin);
    }
    if(p->fd_stdout != STDOUT_FILENO)
    {
        dup2(p->fd_stdout, STDOUT_FILENO);
        close(p->fd_stdout);
    }
    if(p->fd_stderr != STDERR_FILENO)
    {
        dup2(p->fd_stderr, STDERR_FILENO);
        close(p->fd_stderr);
    }
}

Process *destruct_process_pipeline(Process *p)
{
    if(p->next)
        p->next = destruct_process_pipeline(p);
    if(p->argv)
    {
        int i;
        for(i = 0; i < p->argc; i++)
        {
            if(p->argv[i]) free(p->argv[i]);
            p->argv[i] = NULL;
        }
        free(p->argv);
        p->argv = NULL;
    }
    // Clean up the files
    if(p->fd_stdin != STDIN_FILENO
    && p->fd_stdin != STDOUT_FILENO
    && p->fd_stdin != STDERR_FILENO)
        close(p->fd_stdin);
    if(p->fd_stdout != STDIN_FILENO
    && p->fd_stdout != STDOUT_FILENO
    && p->fd_stdout != STDERR_FILENO)
        close(p->fd_stdout);
    if(p->fd_stderr != STDIN_FILENO
    && p->fd_stderr != STDOUT_FILENO
    && p->fd_stderr != STDERR_FILENO)
        close(p->fd_stderr);
    return NULL;
}

// TODO: Handle the built-in functions
void launch_job(Job *j, ForegroundBoolean foreground)
{
    Process *p;
    pid_t forked;
    int little_pipe[2];

    if(!j) return;
    if(!j->process_list) return;
    if(j->state == COMPLETED) return;
    for(p = j->process_list; p; p = p->next)
    {
        // Meet an end for some process
        if(p->state == COMPLETED
        && ((p->status_value != 0 && p->pipeline_discipline == NEXT_IF_SUCCUSS)
            || (p->status_value == 0 && p->pipeline_discipline == NEXT_IF_FAILURE)))
        {
            j->state = COMPLETED;
            break;
        }
        else if(p->state == COMPLETED) continue;
        // Handle pipe
        if(p->is_pipe && p->next)
        {
            if(pipe(little_pipe) < 0)
            {
                print_myshell_err("Failed to create a pipe.");
                exit(1);
            }
            // The output end of the pipe
            p->fd_stdout = little_pipe[1];
            // The input end of the pipe
            p->next->fd_stdin = little_pipe[0];
        }
        forked = fork();
        // Child process
        if(!forked)
        {
            init_child_process(p, j->pgid, foreground);
            // Execute command
            execvp(p->argv[0], p->argv);
            // If failed write the error info
            {
                char buffer[MAX_COMMAND_LEN] = {};
                strcat(buffer, "Failed to launch: ");
                strcat(buffer, p->argv[0]);
                strcat(buffer, ". ");
                print_myshell_err(buffer);
            }
            exit(1);
        }
        // Failed
        else if(forked < 0)
        {
            print_myshell_err("Unable to fork a child process.");
            exit(1);
        }
        // Parent
        else
        {
            p->pid = forked;
            if(INTERACTIVE_MODE)
            {
                // The job process group ID has not been set
                if(!j->pgid)
                    j->pgid = forked;
                // We need to do the same thing twice
                // in case of a racing condition
                setpgid(forked, j->pgid);
            }
            // Clean up the files of the launched process
            if(p->fd_stdin != STDIN_FILENO
            && p->fd_stdin != STDOUT_FILENO
            && p->fd_stdin != STDERR_FILENO)
                close(p->fd_stdin);
            if(p->fd_stdout != STDIN_FILENO
            && p->fd_stdout != STDOUT_FILENO
            && p->fd_stdout != STDERR_FILENO)
                close(p->fd_stdout);
            if(p->fd_stderr != STDIN_FILENO
            && p->fd_stderr != STDOUT_FILENO
            && p->fd_stderr != STDERR_FILENO)
                close(p->fd_stderr);
            // TODO: Print the job
            // If interactive shell
            if(!INTERACTIVE_MODE)
            {
                wait_for_job(j);
            }
            else if(foreground)
                fg_job(j, false);
            else
                bg_job(j, false);
        }
    }
    return;
}

Job *create_job(Process *process_list_, char *command_)
{
    Job *new_job;
    if(!process_list_) return NULL;
    new_job = (Job*)malloc(sizeof(Job));
    if(!new_job) exit(MEM_ALLOC_ERR_);
    new_job->next = NULL;
    new_job->process_list = process_list_;
    new_job->command = (char*)malloc((strlen(command_) + 1) * sizeof(char));
    if(!new_job->command) exit(MEM_ALLOC_ERR_);
    strcpy(new_job->command, command_);
    new_job->pgid = 0;
    new_job->notified = false;
    new_job->tmodes = TERM_ATTR;
    new_job->job_number = global_job_number++;
    new_job->state = UNSTARTED;
    return new_job;
}

Job *create_job_in_list(Process *process_list_, char *command_)
{
    Job *job = create_job_in_list(process_list_, command_);
    Job *j;
    if(!job) return job;
    if(!job_list)
    {
        job_list = job;
        return job;
    }
    for(j = job_list; j->next; j = j->next);
    j->next = job;
    return job;
}

Job *find_job_by_id(int job_number_)
{
    Job *j;
    for(j = job_list; j; j = j->next)
    {
        if(j->job_number == job_number_) return j;
    }
    return NULL;
}

Job *find_job_by_pgid(pid_t pgid_)
{
    Job *j;
    for(j = job_list; j; j = j->next)
    {
        if(j->pgid == pgid_) return j;
    }
    return NULL;
}

void clean_up_jobs()
{
    Job *j;
    for(j = job_list; j; j = j->next)
    {
        refresh_job_status(j);
        if(j->state == COMPLETED)
        {
            job_notify(j);
        }
        // Delete this if notified
        if(j->notified)
        {
            j->process_list = destruct_process_pipeline(j->process_list);
            free(j->command); j->command = NULL;
            if(j->next)
            {
                Job *to_delete_next = j->next;
                // Let this node by its next
                *j = *(j->next);
                // And free the next node
                free(to_delete_next);
                return;
            }
            else // This node is the last node of the linked list
            {
                if(!job_list->next)
                {
                    free(job_list);
                    job_list = NULL;
                    return;
                }
                else
                {
                    Job *jj;
                    for(jj = job_list; jj->next->next; jj = jj->next);
                    free(j);
                    jj->next = NULL;
                    return;
                }
            }
        }
    }
}

void job_notify(Job *j)
{
    if(!j) return;
    refresh_job_status(j);
    if(j->state != COMPLETED) return;
    if(j->notified) return;
    j->notified = true;
    printf("[%d]-\tCompleted\t%s\n", j->job_number, j->command);
    return;
}

// Refresh the status of the processes in the pipeline
void refresh_pipeline_status(Process *p)
{
    if(!p) return;
    for(; p; p = p->next)
    {
        // Temporary status number to receive the status changes from
        // function waitpid
        int status;
        // Return value of waitpid,
        // Return 0 if the process is still ongoing
        // Return the pid if the process is done
        // For the stopped process, the status value is set a special result
        pid_t pid;
        pid = waitpid(p->pid, &status, WUNTRACED|WNOHANG);
        // The process is ongoing
        if(pid <= 0)
        {
            p->state = RUNNING;
            break;
        }
            
        // The process has stopped
        // A temporary special status value is set for the stopped process
        // This is defined in <stdlib.h>
        if(WIFSTOPPED(status))
            p->state = STOPPED;
        // The process is completed
        // And thus the status value of the process in the pipeline is set
        else
        {
            p->state = COMPLETED;
            p->status_value = status;
        }
    }
    return;
}

// Refresh the status of the job
void refresh_job_status(Job *j)
{
    Process *p;
    if(!j) return;
    if(!j->process_list) return;
    refresh_pipeline_status(j->process_list);
    for(p = j->process_list; p; p = p->next)
    {
        if(p->state == STOPPED)
        {
            j->state = STOPPED;
            return;
        }
        if(p->state == COMPLETED
        && ((p->status_value != 0 && p->pipeline_discipline == NEXT_IF_SUCCUSS)
            || (p->status_value == 0 && p->pipeline_discipline == NEXT_IF_FAILURE)))
        {
            j->state = COMPLETED;
            return;
        }
        else if(p->state != COMPLETED)
        {
            if(j->process_list->state != UNSTARTED)
                j->state = RUNNING;
            return;
        }
    }
    j->state = COMPLETED;
    return;
}

// Wait for job
void wait_for_job(Job *j)
{
    while(j->state != STOPPED || j->state != COMPLETED)
        refresh_job_status(j);
}

void fg_job(Job *j, boolean send_sigcont)
{
    if(!j) return;
    if(!j->process_list) return;
    // Put the job to the foreground
    if(j->pgid)
        tcsetpgrp(MYSHELL_TERM_IN, j->pgid);
    // If set, send the job a SIGCONT to let it continue
    if(send_sigcont)
    {
        // This function set up the terminal attributes
        // Option TCSADRAIN, see tcsetattr(3)
        // TCSADRAIN:
        // The change occurs after all output written to fd has been transmitted
        // This option should be used when changing parameters that affect output
        // tcsetattr(MYSHELL_TERM_IN, TCSADRAIN, &(j->tmodes));
        if(j->pgid)
        {
            // Handle the process group
            // Send the sigcont signal to them all
            // The first parameter is set negative to work for the process group
            // See kill(2):
            // If pid is less than -1, then sig is sent to every process in the
            // process group whose ID is -pid
            if(kill(-j->pgid, SIGCONT) < 0)
                print_myshell_err("Error when sending SIGCONT to a process group.");
        }
    }
    // wait for the job
    wait_for_job(j);
    // Now that the job is done
    // Put the process back to the foreground
    tcsetpgrp(MYSHELL_TERM_IN, MYSHELL_PID);
}

void bg_job(Job *j, boolean send_sigcont)
{
    if(!j) return;
    if(!j->process_list) return;
    if(send_sigcont)
    {
        if(j->pgid)
        // I have already written enough in fg_job
            if(kill(-j->pgid, SIGCONT) < 0)
                print_myshell_err("Error when sending SIGCONT to a process group.");
    }
}

static void restore_signals()
{
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);
    signal(SIGTTIN, SIG_DFL);
    signal(SIGTTOU, SIG_DFL);
    signal(SIGCHLD, SIG_DFL);
    return;
}