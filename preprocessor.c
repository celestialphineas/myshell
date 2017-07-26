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
        else return false;
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
    
}