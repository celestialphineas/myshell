#include "preprocessor.h"

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