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
        if(*p == '#')
        {
            in_comment = 1;
            continue;
        }
        else buffer[i++] = *p;
    }
    buffer[i++] = '\n';
    buffer[i] = 0;
    result = (char*)malloc(sizeof(char) * (i + 1));
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
    char buffer[MAX_COMMAND_LEN];
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
            if(p[1] == '\n')
            {
                p++;
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
    result[i] = 0;
    strcpy(result, buffer);
    return result;
}

char **seperate_commands(char *input, int *cmdc)
{
    int in_squote = 0;
    int in_dquote = 0;
    int cmdi = 0;
    char* result[MAX_COMMAND_LINES];
    int i = 0;
    char buffer[MAX_COMMAND_LEN];
    // This pointer scans the input string
    char *p;
    char **allocated_result;

    if(!cmdc)
    {
        cmdc = (int*)malloc(sizeof(int));
    }
    if(!input)
    {
        allocated_result = (char**)malloc(sizeof(char*));
        allocated_result[0] = NULL;
        return allocated_result;
    }
    *cmdc = 0;
    for(p = input; *p; p++)
    {
        if(in_squote)
        {
            if(*p == '\'' && p > input && p[-1] != '\\')
                in_squote = 0;
            buffer[i++] = *p;
            continue;
        }
        if(in_dquote)
        {
            if(*p == '\"' && p > input && p[-1] != '\\')
                in_dquote = 0;
            buffer[i++] = *p;
            continue;
        }
        if(*p == '\'') in_squote = 1;
        if(*p == '\"') in_dquote = 1;
        if((*p == '\n' || *p == ';') && p > input && p[-1] != '\\')
        {
            int is_empty = 1;
            int j;
            for(j = 0; j < i; j++)
            {
                if(!is_blank_char(buffer[j]))
                {
                    is_empty = 0;
                    break;
                }
            }
            buffer[i] = 0;
            // Discard if meets an empty string
            if(is_empty) continue;
            result[cmdi++] = (char*)malloc((i + 1) * sizeof(char));
            strcpy(result[cmdi - 1], buffer);
            i = 0;
            continue;
        }
        else buffer[i++] = *p;
    }
    result[cmdi] = NULL;
    allocated_result = (char**)malloc((cmdi + 1) * sizeof(char*));
    memcpy(allocated_result, result, (cmdi + 1) * sizeof(char*));
    *cmdc = cmdi;
    return allocated_result;
}