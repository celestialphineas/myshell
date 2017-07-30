#include "preprocessor.h"

char BLANK_CHAR[MAX_HOSTNAME_LEN] = " \t";

int is_complete_command(char *cmd_str)
{
    char *p;
    char stack[MAX_COMMAND_LEN];
    int top = -1;
    int in_dquote = 0;
    int in_squote = 0;
    int after_dollar = 0;
    if(cmd_str[strlen(cmd_str)] == '\\'
    || (cmd_str[strlen(cmd_str)] == '\n'
        && cmd_str[strlen(cmd_str) - 1] == '\\'))
            return 0;
    // Priority: ' > \ > " > etc
    for(p = cmd_str; *p; p++)
    {
        if(in_squote)
        {
            after_dollar = 0;
            if(*p == '\'')
                in_squote = 0;
            continue;
        }
        if(*p == '\\')
        {
            after_dollar = 0;
            p++;
            if(!*p) break;
            if(*p == '\n') return 0;
            else continue;
        }
        if(*p == '$')
        {
            after_dollar = 1;
            if(!*p) break;
            else continue;
        }
        if(in_dquote)
        {
            if(*p == '\"')
            {
                after_dollar = 0;
                in_dquote = 0;
                continue;
            }
        }
        if(*p == '\'') in_squote = 1;
        if(*p == '\"') in_dquote = 1;

        if(after_dollar)
        {
            if(*p == '(' || *p == '{' || *p == '[')
                stack[++top] = *p;
        }

        if(*p == ')' && stack[top] == '(')
            top--;
        if(*p == '}' && stack[top] == '{')
            top--;
        if(*p == ']' && stack[top] == '[')
            top--;

        after_dollar = 0;
    }

    if(top >= 0 || (in_dquote | in_squote)) return 0;
    return 1;
}

int is_blank_char(char c)
{
    int i;
    for(i = 0; i < strlen(BLANK_CHAR); i++)
    {
        if(c == BLANK_CHAR[i]) return true;
    }
    return false;
}

char *remove_comments(char *input)
{
    char *p;
    int in_comment = 0;
    char buffer[MAX_COMMAND_LEN];
    int i = 0;
    char *result = NULL;
    if(!input)
    {
        result = (char*)malloc(sizeof(char));
        if(!result) exit(MEM_ALLOC_ERR_);
        result[0] = 0;
        return result;
    }
    for(p = input; *p; p++)
    {
        if(in_comment)
        {
            if(*p == '\n')
            {
                in_comment = 0;
                buffer[i++] = *p;
            }
            continue;
        }
        if((p == input && *p == '#')
        || (p > input && *p == '#' && p[-1] != '$' && p[-1] != '{'))
        {
            in_comment = 1;
            continue;
        }
        else buffer[i++] = *p;
    }
    buffer[i++] = '\n';
    buffer[i] = 0;
    result = (char*)malloc(sizeof(char) * (i + 1));
    if(!result) exit(MEM_ALLOC_ERR_);
    strcpy(result, buffer);
    return result;
}

char *remove_extra_blank(char *input)
{
    int meet_blank = 0;
    int meet_newline = 0;
    int meet_backslash = 0;
    char *p;
    char *result;
    char buffer[MAX_COMMAND_LEN] = {};
    int i = 0;

    for(p = input; *p; p++)
    {
        if(meet_blank)
        {
            if(is_blank_char(*p))
                continue;
        }
        if(meet_newline)
        {
            if(*p == '\n')
                continue;
        }
        if(meet_backslash)
        {
            // The next char
            if(p[0] == '\n')
            {
                p++;
                if(!*p) break;
                buffer[i++] = *p;
                meet_backslash = 0;
                continue;
            }
            else
            {
                buffer[i++] = '\\';
                meet_backslash = 0;
            }
        }
        meet_blank = 0;
        meet_newline = 0;
        if(is_blank_char(*p))
            meet_blank = 1;
        if(*p == '\n')
            meet_newline = 1;
        if(*p == '\\')
        {
            meet_backslash = 1;
            continue;
        }
        buffer[i++] = *p;
    }
    if(buffer[i - 1] != '\n') buffer[i++] = '\n';
    buffer[i] = 0;
    result = (char*)malloc((i + 1) * sizeof(char));
    if(!result) exit(MEM_ALLOC_ERR_);
    result[i] = 0;
    strcpy(result, buffer);
    return result;
}

char **separate_commands(char *input, int *cmdc)
{
    int cmdi = 0;
    char *temp;
    char *processed;
    char* result_buffer[MAX_COMMAND_LINES];
    int i = 0;
    char buffer[MAX_COMMAND_LEN];
    // This pointer scans the input string
    char *p;
    char **result;

    if(!cmdc)
    {
        return NULL;
    }
    if(!input)
    {
        result = (char**)malloc(sizeof(char*));
        if(!result) exit(MEM_ALLOC_ERR_);
        result[0] = NULL;
        if(cmdc) *cmdc = 0;
        return result;
    }

    // Process the input string, remove the comments and the extra spaces
    temp = remove_comments(input);
    processed = remove_extra_blank(temp);
    free(temp);
    temp = NULL;

    buffer[0] = 0;
    result_buffer[0] = NULL;

    for(p = processed; *p; p++)
    {
        if(*p == '\n' || *p == ';')
        {
            int is_empty = 1;
            int j;

            buffer[i] = 0;
            
            for(j = 0; j < i; j++)
            {
                if(!is_blank_char(buffer[j]))
                {
                    is_empty = 0;
                    break;
                }
            }

            // Discard if meets an empty string
            if(is_empty)
            {
                i = 0;
                continue;
            }
            
            // If not a complete command
            if(!is_complete_command(buffer))
            {
                buffer[i++] = *p;
                continue;
            }

            // Else
            result_buffer[cmdi++] = (char*)malloc((i + 1) * sizeof(char));
            if(!result_buffer[cmdi - 1]) exit(MEM_ALLOC_ERR_);
            strcpy(result_buffer[cmdi - 1], buffer);
            i = 0;
            continue;
        }
        else buffer[i++] = *p;
    }
    result_buffer[cmdi] = NULL;
    // Unfinished line occurs at the end of the file
    if(i != 0)
    {
        int j;
        *cmdc = -1;
        for(j = 0; j < cmdi; j++)
            if(result_buffer[i]) free(result_buffer[i]);
        print_myshell_err("Unexpected end of file. \
Please check syntax of the input file.");
        return NULL;
    }
    result = (char**)malloc((cmdi + 1) * sizeof(char*));
    if(!result) exit(MEM_ALLOC_ERR_);
    memcpy(result, result_buffer, (cmdi + 1) * sizeof(char*));
    *cmdc = cmdi;
    return result;
}