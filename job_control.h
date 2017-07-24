#ifndef CELPHI_JOB_CONTROL_H
#define CELPHI_JOB_CONTROL_H
#include "global.h"
#include "message.h"

typedef enum
{
    NORMAL = 1,
    NEXT_IF_SUCCUSS = 2,
    NEXT_IF_FAILURE = 3
} PipelineDiscipline;

typedef enum
{
    UNSTARTED = 0,
    COMPLETED = 1,
    STOPPED = 2,
    RUNNING = 3
} State;

typedef struct Process
{
    struct Process *next;
    int argc;
    char **argv;
    pid_t pid;
    int status_value;
    boolean is_pipe;
    PipelineDiscipline pipeline_discipline;
    int fd_stdin;
    int fd_stdout;
    int fd_stderr;
    boolean append_out;
    boolean append_err;
    State state;
} Process;

typedef struct Job
{
    struct Job *next;
    Process *process_list;
    char *command;
    pid_t pgid;
    boolean notified;
    struct termios tmodes;
    int job_number;
    State state;
} Job;

extern Job *job_list;

Process *create_process(int argc_, char **argv_,
    boolean is_pipe_, boolean append_out_, boolean append_err_,
    PipelineDiscipline pipeline_discipline_,
    char *in_file, char *out_file, char *err_file,
    int in_file_fd, int out_file_fd, int err_file_fd);

typedef enum {BACKGROUND = 0, FORGROUND = 1} ForegroundBoolean;

// This function updates the status of processes in a pipeline
void refresh_pipeline_status(Process*);
// And this updates the processes in a job
// and the job itself
void refresh_job_status(Job*);
// Wait for a job to finishe
// This has not to set myshell back to the foreground
void wait_for_job(Job*);
// Put a process to the foregroud
void fg_job(Job*, boolean send_sigcont);
// Put a process to the background
void bg_job(Job*, boolean send_sigcont);

#endif