#include "tokenizer.h"

char CONTROL_CHAR[MAX_HOSTNAME_LEN] = "&|()";

int is_control_char(char c)
{
    int i;
    for(i = 0; i < strlen(CONTROL_CHAR); i++)
    {
        if(c == CONTROL_CHAR[i]) return true;
    }
    return false;
}

char *raw_tokenize(char *input, int *len)
{
    int i, j;
    int in_squote = 0;
    int in_dquote = 0;
    char buffer[MAX_COMMAND_LEN];
    char *result;
    
    // Exceptions
    if(!input)
    {
        if(len) *len = 0;
        result = (char*)malloc(sizeof(char));
        if(!result) exit(MEM_ALLOC_ERR_);
        result[0] = 0;
        return result;
    }
    if(!len) return NULL;

    // See int is_complete_command(char*) in preprocessor.c
    for(i = 0, j = 0; i <= strlen(input); i++)
    {
        if(in_squote)
        {
            if(input[i] == '\'')
                in_squote = 0;
            buffer[j++] = input[i];
            continue;
        }
        if(input[i] == '\\')
        {
            buffer[j++] = input[i++];
            if(i > strlen(input)) break;
            continue;
        }
        if(in_dquote)
        {
            if(input[i] == '\"')
                in_dquote = 0;
            buffer[j++] = input[i];
            continue;
        }
        if(input[i] == '\'') in_squote = 1;
        if(input[i] == '\"') in_dquote = 1;
        if(is_blank_char(input[i]))
        {
            buffer[j++] = 0;
            continue;
        }
        if(is_control_char(input[i]))
        {
            if(i > 0 && !is_control_char(input[i - 1])
                && !is_blank_char(input[i - 1]))
                buffer[j++] = 0;
            buffer[j++] = input[i];
            if(i < strlen(input) && !is_control_char(input[i + 1])
                && !is_blank_char(input[i+1]))
                    buffer[j++] = 0;
            continue;
        }
        buffer[j++] = input[i];
    }
    
    // The result is j-character long
    result = (char*)malloc(j * sizeof(char));
    memcpy(result, buffer, j * sizeof(char));
    *len = j;
    return result;
}