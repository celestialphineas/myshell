#include "job_control.h"

extern int LATEST_STATUS;

// The restore signals procedure is written for the child processes
static void restore_signals();
// The restore control procedure is written for the parent process
static void restore_control();

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
        // if(access(out_file, W_OK) != 0)
        // {
        //     char buffer[MAX_COMMAND_LEN] = {};
        //     strcat(buffer, out_file);
        //     strcat(buffer, " is not write accessible.");
        //     print_myshell_err(buffer);
        //     free(new_process);
        //     return NULL;
        // }
        // else // the file is write accessible
        // {
            int fd;
            if(append_out_)
                fd = open(out_file, O_WRONLY|O_CREAT|O_APPEND, 0666);
            else
                fd = open(out_file, O_WRONLY|O_CREAT, 0666);
            new_process->fd_stdout = fd;
        // }
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
                fd = open(err_file, O_WRONLY|O_CREAT|O_APPEND, 0666);
            else
                fd = open(err_file, O_WRONLY|O_CREAT, 0666);
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
void init_child_process(Process *p, pid_t *pgid, ForegroundBoolean foreground)
{
    if(!p) return;
    if(p->argc == 0 || !p->argv || !p->argv[0]) return;
    if(!pgid) return;
    if(INTERACTIVE_MODE)
    {
        // Set pgid
        p->pid = getpid();
        if(!*pgid) *pgid = p->pid;
        if(setpgid(0, *pgid) != 0)
        {
            *pgid = p->pid;
            setpgid(0, 0);
        }
        // If foreground
        // Set the terminal's control process group
        if(foreground)
        {
            tcsetpgrp(MYSHELL_TERM_IN, *pgid);
            tcsetattr(MYSHELL_TERM_IN, TCSADRAIN, &TERM_ATTR);
        }
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
    // Execute command
    execvp(p->argv[0], p->argv);
}

// These are used to store the standard input and outputs
static int dup_stdin = -1;
static int dup_stdout = -1;
static int dup_stderr = -1;
static void save_std()
{
    if(dup_stdin == -1)
        dup_stdin = dup(STDIN_FILENO);
    if(dup_stdout == -1)
        dup_stdout = dup(STDOUT_FILENO);
    if(dup_stderr == -1)
        dup_stderr = dup(STDERR_FILENO);
    return;
}
static void restore_std()
{
    if(dup_stdin != -1)
    {
        fflush(stdin);
        dup2(dup_stdin, STDIN_FILENO);
        close(dup_stdin);
        dup_stdin = -1;
    }
    if(dup_stdout != -1)
    {
        fflush(stdout);
        dup2(dup_stdout, STDOUT_FILENO);
        close(dup_stdout);
        dup_stdout = -1;
    }
    if(dup_stderr != -1)
    {
        fflush(stderr);
        dup2(dup_stderr, STDERR_FILENO);
        close(dup_stderr);
        dup_stderr = -1;
    }
    return;
}

// Handle the built-ins
void init_built_in(Process *p)
{
    if(!p) return;
    if(p->argc == 0 || !p->argv || !p->argv[0]) return;
    // The behavior of the built-ins are always the same
    // No matter myshell is running in interactive mode or not
    // And have nothing to do with the fg/bg status
    p->pid = MYSHELL_PID;
    // No need to set the terminal controling process group
    // Restore the signals
    restore_signals();
    // Save the std I/O
    save_std();
    // Set the files, for pipes and redirections
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
    // Execute command
    // For this part, see built_in.h for usage
    {
        BuiltIn *built_in;
        built_in = get_built_in(p->argv[0]);
        if(!built_in)
        {
            print_myshell_err("An internal error of some built-in occurs. ");
            exit(BUILT_IN_RUNTIME_ERR_);
        }
        // run!
        p->status_value = (*built_in)(p->argc, p->argv);
        LATEST_STATUS = p->status_value;
        p->state = COMPLETED;
    }
    return;
}

Process *destruct_process_pipeline(Process *p)
{
    if(!p) return NULL;
    if(p->next)
        p->next = destruct_process_pipeline(p->next);
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
        // If not a built-in, fork
        if(!is_built_in(p->argv[0]))
        {
            char parent[MAX_PATH_LEN];
            strcpy(parent, MYSHELL_PATH);
            strcat(parent, "/myshell");
            setenv("PARENT", parent, 1);
            forked = fork();
        }
        else
        {
            init_built_in(p);
            continue;
        }
        // Child process
        if(!forked)
        {
            init_child_process(p, &(j->pgid), foreground);
            
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
            // The job process group ID has not been set
            if(!j->pgid)
                j->pgid = forked;
            // We need to do the same thing twice
            // in case of a racing condition
            if(setpgid(forked, j->pgid) != 0)
            {
                setpgid(forked, forked);
                j->pgid = forked;
            }
            // If not running in interactive mode
            // or the job is launched as a forground job
            // or the process in the pipeline must be blocked
            // because of its pipelining discipline
            if(!INTERACTIVE_MODE || foreground || p->pipeline_discipline != NORMAL)
            {
                int status;
                // pid_t pid;
                if(!foreground)
                    restore_control();
                /*pid = */waitpid(p->pid, &status, WUNTRACED);
                // If the job is stopped, stop the whole pipeline
                // TODO: To print something
                if(WIFSTOPPED(status))
                {
                    p->state = STOPPED;
                    p->status_value = WIFSTOPPED(status);
                    j->state = STOPPED;
                    break;
                }
                else
                {
                    p->state = COMPLETED;
                    p->status_value = status;
                    LATEST_STATUS = status;
                }
            }
            else
            {
                // TODO: Create background job, print something
                restore_control();
            }
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
    }
    // To turn off the foreground notification
    if(!INTERACTIVE_MODE || foreground)
            j->notified = true;
    restore_std();
    restore_control();
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
    Job *job = create_job(process_list_, command_);
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

Process *find_process_by_pid(pid_t pid_)
{
    Job *j;
    Process *p;
    for(j = job_list; j; j = j->next)
        for(p = job_list->process_list; p; p = p->next)
            if(p->pid == pid_)
                return p;
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
        if(j->state == COMPLETED && j->notified)
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
        if(p->state == UNSTARTED)
        {
            // Temporary status number to receive the status changes from
            // function waitpid
            int status = 0;
            // Return value of waitpid,
            // Return 0 if the process is still ongoing
            // Return the pid if the process is done
            // For the stopped process, the status value is set a special result
            pid_t pid;
            pid = waitpid(p->pid, &status, WUNTRACED | WNOHANG);
            // The process terminated
            if(pid == -1)
            {
                // The process is running
                if(WIFSTOPPED(status))
                {
                    p->state = STOPPED;
                    p->status_value = status;
                }
                else if(!kill(pid, 0))
                    p->state = RUNNING;
                else // The process does not exist, i.e. terminated
                    p->state = COMPLETED;
            }
            else if(pid == 0)
            {
                if(WIFSTOPPED(status))
                {
                    p->state = STOPPED;
                    p->status_value = status;
                }
                else if(!kill(pid, 0))
                    p->state = RUNNING;
                else
                {
                    p->state = COMPLETED;
                    p->status_value = status;
                }
            }
            else if(WIFSTOPPED(status))
            {
                p->state = STOPPED;
                p->status_value = status;
            }
            else
            {
                p->state = COMPLETED;
                p->status_value = status;
            }
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
    p = j->process_list;
    if(p->state == UNSTARTED)
    {
        j->state = UNSTARTED;
        return;
    }
    for(p = j->process_list; p; p = p->next)
    {
        if(p->state == STOPPED)
        {
            j->state = STOPPED;
            return;
        }
        if(p->state == RUNNING)
        {
            j->state = RUNNING;
            return;
        }
        if(p->state == COMPLETED
        && ((p->status_value != 0 && p->pipeline_discipline == NEXT_IF_SUCCUSS)
            || (p->status_value == 0 && p->pipeline_discipline == NEXT_IF_FAILURE)))
        {
            j->state = COMPLETED;
            return;
        }
        if(p->state != COMPLETED)
        {
            if(j->process_list->state != UNSTARTED)
                j->state = RUNNING;
            return;
        }
    }
    j->state = COMPLETED;
    return;
}

void fg_job(Job *j)
{
    Process *p;
    if(!j) return;
    if(!j->process_list) return;
    refresh_job_status(j);
    if(j->state == COMPLETED) return;
    // Put the job to the foreground
    if(j->pgid)
        tcsetpgrp(MYSHELL_TERM_IN, j->pgid);
    for(p = j->process_list; p; p = p->next)
    {
        if(p->state == COMPLETED
        && ((p->status_value != 0 && p->pipeline_discipline == NEXT_IF_SUCCUSS)
            || (p->status_value == 0 && p->pipeline_discipline == NEXT_IF_FAILURE)))
        {
            j->state = COMPLETED;
            break;
        }
        if(p->state == COMPLETED) continue;
        else if(p->state == STOPPED)
        {
            // If failed to kill
            if(kill(-p->pid, SIGCONT))
            {
                print_myshell_err("Error when sending SIGCONT to a process group.");
                j->state = COMPLETED;
                break;
            }
            // Success to send SIGCONT to the process
            // Do something similar to the launching
            int status;
            // pid_t pid;
            /*pid = */waitpid(p->pid, &status, WUNTRACED);
            if(WIFSTOPPED(status))
            {
                p->state = STOPPED;
                p->status_value = WIFSTOPPED(status);
                j->state = STOPPED;
                break;
            }
            else
            {
                p->state = COMPLETED;
                p->status_value = status;
                j->state = RUNNING;
                break;
            }
        }
        else if(p->state == RUNNING)
        {
            int status;
            // pid_t pid;
            /*pid = */waitpid(p->pid, &status, WUNTRACED);
            // If the job is stopped, stop the whole pipeline
            if(WIFSTOPPED(status))
            {
                p->state = STOPPED;
                p->status_value = WIFSTOPPED(status);
                j->state = STOPPED;
                restore_control();
                return;
            }
            else
            {
                p->state = COMPLETED;
                p->status_value = status;
            }
        }
        else if(p->state == UNSTARTED)
        {
            pid_t forked;
            int little_pipe[2];
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
            if(!is_built_in(p->argv[0]))
            {
                char parent[MAX_PATH_LEN];
                strcpy(parent, MYSHELL_PATH);
                strcat(parent, "/myshell");
                setenv("PARENT", parent, 1);
                forked = fork();
            }
            else
            {
                init_built_in(p);
                continue;
            }
            // Child process
            if(!forked)
            {
                init_child_process(p, &(j->pgid), FORGROUND);
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
                // The job process group ID has not been set
                if(!j->pgid)
                    j->pgid = forked;
                // We need to do the same thing twice
                // in case of a racing condition
                if(setpgid(forked, j->pgid) != 0)
                {
                    setpgid(forked, forked);
                    j->pgid = forked;
                }
                {
                    int status;
                    // pid_t pid;
                    /*pid = */waitpid(p->pid, &status, WUNTRACED);
                    // If the job is stopped, stop the whole pipeline
                    if(WIFSTOPPED(status))
                    {
                        p->state = STOPPED;
                        p->status_value = WIFSTOPPED(status);
                        j->state = STOPPED;
                        restore_control();
                        return;
                    }
                    else
                    {
                        p->state = COMPLETED;
                        p->status_value = status;
                    }
                }
            }
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
    }
    j->state = COMPLETED;
    restore_control();
}

void bg_job(Job *j)
{
    Process *p;
    if(!j) return;
    if(!j->process_list) return;
    refresh_job_status(j);
    if(j->state == COMPLETED) return;
    for(p = j->process_list; p; p = p->next)
    {
        if(p->state == COMPLETED) continue;
        if(p->state == STOPPED)
        {
            if(kill(-p->pid, SIGCONT))
            {
                print_myshell_err("Error when sending SIGCONT to a process group.");
                j->state = COMPLETED;
                break;
            }
        }
        else
        {
            init_child_process(p, &j->pgid, BACKGROUND);
        }
    }
    j->state = RUNNING;
    j->notified = false;
    return;
}

void sigchld_handler(int signum)
{
    int status;
    pid_t pid;
    Process *p;
    pid = waitpid(-1, &status, WNOHANG | WUNTRACED);
    p = find_process_by_pid(pid);
    if(p)
    {
        p->status_value = status;
        if(WIFSTOPPED(status)) p->state = STOPPED;
        else p->state = COMPLETED;
    }
    return;
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

static void restore_control()
{
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    // Child process signal handler
    signal(SIGCHLD, sigchld_handler);
    tcsetpgrp(MYSHELL_TERM_IN, MYSHELL_PID);
    tcsetattr(MYSHELL_TERM_IN, TCSADRAIN, &TERM_ATTR);
    return;
}
