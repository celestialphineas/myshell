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
// If an error occur, return NULL
// The first slot, dollar_pos is the pointer to '$' in the string
// After this procedure is done,
// The last character of the expanded sequence is passed in end_pos
char *expand_myshell_argc(char *dollar_pos, char **end_pos)
{
    char buffer[MAX_HOSTNAME_LEN] = {};
    int arg_count;
    char *result;

    if(!dollar_pos || !end_pos) return NULL;
    for(arg_count = 0; ; arg_count++)
    {
        // This is the string in which stores the number
        char query_num_str[MAX_HOSTNAME_LEN];
        char *varv;
        sprintf(query_num_str, "%d", arg_count);
        varv = get_variable(query_num_str, 0);
        if(!varv || strlen(varv) == 0)
        {
            if(varv) free(varv);
            break;
        }
        free(varv);
    }
    arg_count++;
    sprintf(buffer, "%d", arg_count);
    result = (char*)malloc((strlen(buffer) + 1) * sizeof(char));
    if(!result) exit(MEM_ALLOC_ERR_);
    strcpy(result, buffer);
    *end_pos = dollar_pos + 1;
    return result;
}

// Still the return value is the expanded string with allocated space
// See definition of expand_myshell_argc
char *expand_myshell_argv(char *dollar_pos, char **end_pos)
{
    char buffer[MAX_COMMAND_LEN] = {};
    int arg_count;
    char *result;

    if(!dollar_pos || !end_pos) return NULL;
    for(arg_count = 0; ; arg_count++)
    {
        // This is the string in which stores the number
        char query_num_str[MAX_HOSTNAME_LEN];
        char *varv;
        sprintf(query_num_str, "%d", arg_count);
        varv = get_variable(query_num_str, 0);
        if(!varv || strlen(varv) == 0)
        {
            if(varv) free(varv);
            break;
        }
        // append
        strcat(buffer, varv);
        strcat(buffer, " ");
        free(varv);
    }
    // Remove the last space
    if(*buffer) buffer[strlen(buffer) - 1] = 0;
    result = (char*)malloc((strlen(buffer) + 1) * sizeof(char));
    if(!result) exit(MEM_ALLOC_ERR_);
    strcpy(result, buffer);
    *end_pos = dollar_pos + 1;
    return result;
}

char *expand_myshell_arg(char *dollar_pos, char **end_pos)
{
    char digit[2] = {};

    if(!dollar_pos || !end_pos) return NULL;
    digit[0] = dollar_pos[1];
    return get_variable(digit, 0);
}

char *expand_myshell_unbraced_var(char *dollar_pos, char **end_pos)
{
    char key_buffer[MAX_HOSTNAME_LEN] = {};
    int i = 0;
    char *p = dollar_pos;
    
    if(!dollar_pos || !end_pos) return NULL;
    
    // The first element of an identifier
    key_buffer[i++] = *p;
    // Read in the key, greedily
    while(*p)
    {
        if(isalpha(*p) || isdigit(*p) || *p == '_')
        {
            key_buffer[i++] = *p;
            p++;
        }
        else break;
    }

    *end_pos = p - 1;
    return get_variable(key_buffer, 0);
}

// This is the major function to handle things like ${}
// Damn it!
char *expand_myshell_var(char *dollar_pos, char **end_pos)
{
    char *p = dollar_pos;
    char braced_buffer[MAX_HOSTNAME_LEN] = {};
    char bracket_buffer[MAX_HOSTNAME_LEN] = {};
    int i = 0;
    int sharp = 0;
    int all_elements = 0;
    int index_of_element = 0;
    char *left_bracket_pos;
    char *right_bracket_pos;
    char left_bracket[2] = {'['};
    char right_bracket[2] = {']'};

    if(!dollar_pos || !end_pos) return NULL;

    // Read in the braced part
    p += 2;
    while(*p != '}' && *p)
    {
        braced_buffer[i++] = *p;
        p++;
    }
    *end_pos = p;

    // Judge if the first character is a sharp
    if(braced_buffer[0] == '#')
    {
        sharp = 1;
        strcpy(braced_buffer, braced_buffer + 1);
    }

    // Find the brackets
    left_bracket_pos = strstr(braced_buffer, left_bracket);
    right_bracket_pos = strstr(braced_buffer, right_bracket);
    // Left and right do not match
    if((left_bracket_pos || right_bracket_pos)
        && !(left_bracket_pos && right_bracket_pos))
            return NULL;
    // Get the string in the brackets
    if(left_bracket_pos && right_bracket_pos)
    {
        memcpy(bracket_buffer, left_bracket_pos,
            right_bracket_pos - left_bracket_pos - 1);
        bracket_buffer[right_bracket_pos - left_bracket_pos - 1] = 0;
    }
    // If the string in the brackets indicates an all_element
    if(bracket_buffer[0] == '*' || bracket_buffer[0] == '@')
        all_elements = 1;
    // Else if it indicates an index
    else if(isdigit(bracket_buffer[0]))
        sscanf(bracket_buffer, "%d", &index_of_element);
    // Else it is illegal
    else return NULL;

    // Make the braced buffer stores the identifier only
    if(left_bracket_pos) *left_bracket_pos = 0;

    // Clean up the braced buffer, remove the blank characters
    {
        char *blank_removed = remove_extra_blank(braced_buffer);
        if(blank_removed) strcpy(braced_buffer, blank_removed);
        else return NULL;
        free(blank_removed);
        blank_removed = NULL;
        if(!is_identifier(braced_buffer)) return NULL;
    }

    // If it is to find out the number of elements in a list
    if(all_elements && sharp)
    {
        return get_variable(braced_buffer, ElementCount);
    }
    else if(sharp)
    {
        char *result = get_variable(braced_buffer, index_of_element);
        char result_buffer[MAX_HOSTNAME_LEN];
        if(!result) return NULL;
        sprintf(result_buffer, "%d", (int)strlen(result));
        free(result);
        result = (char*)malloc((strlen(result_buffer) + 1) * sizeof(char));
        if(!result) exit(MEM_ALLOC_ERR_);
        strcpy(result, result_buffer);
        return result;
    }
    else
    {
        if(all_elements) index_of_element = ListAll;
        return get_variable(braced_buffer, index_of_element);
    }
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
                // TODO
                continue;
            }
            else if(is_myshell_argv(p))
            {
                // TODO
                continue;
            }
            else if(is_myshell_arg(p))
            {
                // TODO
                continue;
            }
            else if(is_myshell_unbraced_var(p))
            {
                // TODO
                continue;
            }
            else if(is_myshell_var(p))
            {
                // TODO
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