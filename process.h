// Process
// =======
// Author: Celestial Phineas @ ZJU
//         (Yehang YIN)
// This file implements the process group control and launch
// The data structure to describe the process control is given
#ifndef CELPHI_PROCESS_H
#define CELPHI_PROCESS_H
#include "global.h"
#include "message.h"

typedef enum
{
    UNSTARTED = 0,
    COMPLETED = 1,
    STOPPED = 2,
    RUNNING = 3
} ProcessState;

typedef enum
{
    NORMAL = 1,
    NEXT_IF_SUCCUSS = 2,
    NEXT_IF_FAILURE = 3
} PipelineDiscipline;

// The process as well as the process pipeline data structure
// ==========================================================
// next     - Next process in the pipeline
// argc     - Argument count
// argv     - Argument value
// pid      - Pid
// pgid     - Process group ID
// proces_state     - Current process state
// status_value     - Return status value
// is_pipe          - If the process creates a pipe
// pipeline_discipline  - && or ||
// fd_stdin     - File descriptor for stdin
// fd_stdout    - File descriptor for stdout
// fd_stderr    - File descriptor for stderr
// append_out   - Output mode: append or 
//
// Head of this linked list is the process group leader.
typedef struct Process
{
    struct Process *next;
    int argc;
    char **argv;
    pid_t pid;
    pid_t pgid;
    ProcessState process_state;
    int status_value;
    boolean is_pipe;
    PipelineDiscipline pipeline_discipline;
    int fd_stdin;
    int fd_stdout;
    int fd_stderr;
    boolean append_out;
} Process;

// ProcessPipeline is a series of processes
typedef Process *ProcessPipeline;

Process *create_process(int argc, char **argv,
    boolean is_pipe_, boolean append_out_,
    PipelineDiscipline pipeline_discipline_,
    char *in_file, char *out_file, char *err_file,
    int in_file_fd, int out_file_fd, int err_file_fd);

typedef enum {BACKGROUND = 0, FORGROUND = 1} ForegroundBoolean;

// Launch/resume the process pipeline
int launch_process_pipeline(ProcessPipeline, boolean foreground);

void fg_process_group(Process*);
void bg_process_group(Process*);

Process *destruct_process_pipeline(ProcessPipeline);

#endif