// Command parser
// ==============
// Author: Celestial Phineas
//         (Yehang YIN)
// Command parser
// This file convert the command line string to a job
#ifndef CELPHI_PARSER_H
#define CELPHI_PARSER_H
#include "tokenizer.h"
#include "expansion.h"
#include "job_control.h"

// Argument cmd must be preprocessed with separate_commands
// Input of this slot must be a pipeline command
Job *command_to_job(char *cmd, int *found_bg);

#endif