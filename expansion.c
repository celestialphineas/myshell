#include "expansion.h"

int is_escaped(char *ch, char *head)
{
    char *p;
    int backslash_count = 0;
    if(!ch || !head || ch <= head) return false;
    
    for(p = ch - 1; p >= head; p--)
    {
        if(*p == '\\') backslash_count++;
        else break;
    }
    if(backslash_count%2) return true;
    else return false;
}

char *tilde_expansion(char *str)
{
    int i = 0;
    // It is ensured that the tilde expansion is done only once
    int done = 0;
    char buffer[MAX_COMMAND_LEN] = {};
    char *p;
    char *result;

    for(p = str; *p; p++)
    {
        if(*p == '~' && !is_escaped(p, str) &&!done)
        {
            if(p == str && p[1] == 0)
            {
                // [^]~[\0]
                strcat(buffer, "${HOME}");
                i += strlen("${HOME}");
                done = 1;
                continue;
            }
            else if(p[1] == '/')
            {
                // ~/
                strcat(buffer, "${HOME}");
                i += strlen("${HOME}");
                done = 1;
                continue;
            }
            else if(p == str && p[1] == '+' && p[2] == 0)
            {
                // [^]~+[\0]
                strcat(buffer, "${PWD}");
                i += strlen("${PWD}");
                p++;
                done = 1;
                if(!*p) break;
                continue;
            }
            else if(p[1] == '+' && p[2] == '/')
            {
                // ~+/
                strcat(buffer, "${PWD}");
                i += strlen("${PWD}");
                p++;
                done = 1;
                if(!*p) break;
                continue;
            }
        }
        buffer[i++] = *p;
    }

    result = (char*)malloc((strlen(buffer) + 1) * sizeof(char));
    strcpy(result, buffer);
    return result;
}

// $#
int is_myshell_argc(char *str)
{
    if(!str || !*str) return 0;
    if(str[0] == '$' && str[1] == '#') return 1;
    return 0;
}

// $[@|*]
int is_myshell_argv(char *str)
{
    if(!str || !*str) return 0;
    if(str[0] == '$' && (str[1] == '@' || str[1] == '*'))
        return 1;
    return 0;
}

// $[digit]
int is_myshell_arg(char *str)
{
    if(!str || !*str) return 0;
    if(str[0] == '$' && isdigit(str[1])) return 1;
    return 0;
}

// $[identifier]
int is_myshell_unbraced_var(char *str)
{
    if(!str || !*str) return 0;
    if(str[0] == '$' && (isalpha(str[1]) || str[1] == '_'))
        return 1;
    return 0;
}

// ${[string]}
int is_myshell_var(char *str)
{
    int matched_brace = 0;
    if(!str || !*str) return 0;
    if(str[0] != '$' || !str[1]) return 0;
    str++;
    if(*str != '{') return 0;
    while(*str)
    {
        if(*str == '}') return 1;
        str++;
    }
    return 0;
}

// The return value is the expanded string with allocated space
// The first slot, dollar_pos is the pointer to '$' in the string
// After this procedure is done,
// The last character of the expanded sequence is passed in end_pos
char *expand_myshell_argc(char *dollar_pos, char **end_pos)
{
    return 0;
}

char *var_expansion(char *str)
{
    int i = 0;
    char buffer[MAX_COMMAND_LEN] = {};
    char *p;
    char *result;

    for(p = str; *p; p++)
    {
        // Start of the expansion sequence
        if(*p == '$' && !is_escaped(p, str))
        {
            if(is_myshell_argc(p))
            {
                continue;
            }
            else if(is_myshell_argv(p))
            {
                continue;
            }
            else if(is_myshell_arg(p))
            {
                continue;
            }
            else if(is_myshell_unbraced_var(p))
            {
                continue;
            }
            else if(is_myshell_var(p))
            {
                continue;
            }
            // Else simply output the string
        }
        buffer[i++] = *p;
    }

    result = (char*)malloc((strlen(buffer) + 1) * sizeof(char));
    strcpy(result, buffer);
    return result;
}