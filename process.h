// Process
// =======
// Author: Celestial Phineas @ ZJU
//         (Yehang YIN)
#ifndef CELPHI_PROCESS_H
#define CELPHI_PROCESS_H
#include "global.h"
#include "message.h"

typedef enum
{
    UNSTARTED = 0,
    COMPLETED = 1,
    STOPPED = 2
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

typedef Process *ProcessPipeline;

#endif