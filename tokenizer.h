// Tokenizer
// =========
// Author: Celestial Phineas @ ZJU
//         (Yehang YIN)
// This file implements a simple tokenizer for myshell
#ifndef CELPHI_TOKENIZER_H
#define CELPHI_TOKENIZER_H
#include "preprocessor.h"

extern char CONTROL_CHAR[];
// Token data structure
// splitters (as shown in the bash reference)
// keywords
// > < && & || | and others
// string quoted in single quotation marks
// string quoted in double quotation marks
// subshell
// normal

// PIPE             |
// PIPELINE_SEP     && ||
// SQUOTED          ''
// DQUOTED          ""
// SET_EXPR         <identifier>=<identifier>
// UNQUOTED         Ordinary strings
typedef enum
{
    CONTROL,
    SQUOTED,
    DQUOTED,
    SET_EXPR,
    UNQUOTED
} TokenType;

typedef struct
{
    char *value;
    TokenType type;
} Token;

int is_control_char(char c);
// You have to ensure that the input string is preprocessed
// The raw tokenizer transform the input string to a series of seperated tokens
// in the returned string. Use '\0' as the separator and the filler.
// Length of the returned string is stored in len.
// Return an empty string if the input is NULL
// Return NULL if an error occurs.
char *raw_tokenize(char *input, int *len);

#endif