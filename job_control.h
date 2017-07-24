// Job and process control
// =======================
// Author: Celestial Phineas
//         (Yehang YIN)
// This file implements the job control feature of myshell
// Also handles the process pipeline
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

// The process as well as the process pipeline data structure
// ==========================================================
// next     - Next process in the pipeline
// argc     - Argument count
// argv     - Argument value
// pid      - Pid
// pgid     - Process group ID
// state    - Current process state
// status_value         - Return status value
// is_pipe              - If the process creates a pipe
// pipeline_discipline  - && or ||
// fd_stdin     - File descriptor for stdin
// fd_stdout    - File descriptor for stdout
// fd_stderr    - File descriptor for stderr
// append_out   - Output mode: whether open the output file in appending mode
// append_err   - Error output mode: whether open the error file in appending mode

// Head of this linked list is the process group leader.
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

// When creating a process,
// All the arguments are made a copy
Process *create_process(int argc_, char **argv_,
    boolean is_pipe_, boolean append_out_, boolean append_err_,
    PipelineDiscipline pipeline_discipline_,
    char *in_file, char *out_file, char *err_file,
    int in_file_fd, int out_file_fd, int err_file_fd);
// The destruct process pipeline method destructs the process list
Process *destruct_process_pipeline(Process *p);

// ProcessPipeline is a series of process
typedef Process *ProcessPipeline;

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

// There is a global access point for the job list
extern Job *job_list;

// Create a job. It adopts the process list and make shallow copy of it
// However it makes a copy of command.
// Handle with care
Job *create_job(Process *process_list, char *command);
// And this one create the job in the global job list
Job *create_job_in_list(Process *process_list, char *command);
// Find the job by its job_number
Job *find_job_by_id(int job_number_);
// Find the job by its pgid
Job *find_job_by_pgid(pid_t pgid_);
// Clean up the completed jobs
void clean_up_jobs();
// Job notify
void job_notify(Job*);

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