#include "tokenizer.h"

char CONTROL_CHAR[MAX_HOSTNAME_LEN] = "&|()<>";

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

int is_identifier(char *str)
{
    char *p = str;
    if(!str) return false;
    if(!isalpha(*str) && *str != '_') return false;
    while(*p)
    {
        if(!isalpha(*p) && !isdigit(*p) && *p != '_')
            return false;
        p++;
    }
    return true;
}

int is_control_sequence(char *str)
{
    char *p = str;
    if(!str) return false;
    while(*p)
    {
        if(!is_control_char(*p)) return false;
        p++;
    }
    return true;
}

int is_squoted_string(char *str)
{
    if(!str) return false;
    if(str[0] == '\'' && str[strlen(str) - 1] == '\'') return true;
    return false;
}

int is_dquoted_string(char *str)
{
    if(!str) return false;
    if(str[0] == '\"' && str[strlen(str) - 1] == '\"') return true;
    return false;
}

int is_set_expr(char *str)
{
    int i;
    char buffer[MAX_COMMAND_LEN];
    int equal_pos = 0;
    for(i = 0; i < strlen(str); i++)
    {
        if(str[i] == '=')
        {
            equal_pos = i;
            break;
        }
    }
    if(!equal_pos) return false;
    memcpy(buffer, str, equal_pos);
    buffer[equal_pos] = 0;
    if(!is_identifier(buffer)) return false;
    return true;
}

Token **tokenize(char *input, int *tokenc)
{
    Token **result;
    Token token_buffer[MAX_TOKEN_NUM];
    char buffer[MAX_COMMAND_LEN];
    char *raw_tokenized;
    int raw_len;
    int i, j;

    if(!input)
    {
        if(tokenc) *tokenc = 0;
        return NULL;
    }
    else if(!tokenc) return NULL;

    raw_tokenized = raw_tokenize(input, &raw_len);
    if(!raw_tokenized) return NULL;

    for(i = 0, j = 0; i < raw_len; )
    {
        strcpy(buffer, raw_tokenized + i);
        if(strlen(buffer) > 0)
        {
            if(is_control_sequence(buffer))
            {
                token_buffer[j++].value
                    = (char*)malloc((strlen(buffer) + 1) * sizeof(char));
                if(!token_buffer[j-1].value) exit(MEM_ALLOC_ERR_);
                strcpy(token_buffer[j-1].value, buffer);
                token_buffer[j-1].type = CONTROL;
            }
            else if(is_squoted_string(buffer))
            {
                token_buffer[j++].value
                    = (char*)malloc((strlen(buffer) - 1) * sizeof(char));
                if(!token_buffer[j-1].value) exit(MEM_ALLOC_ERR_);
                memcpy(token_buffer[j-1].value, buffer + 1, strlen(buffer) - 2);
                token_buffer[j-1].value[strlen(buffer) - 1] = 0;
                token_buffer[j-1].type = SQUOTED;
            }
            else if(is_dquoted_string(buffer))
            {
                token_buffer[j++].value
                    = (char*)malloc((strlen(buffer) - 1) * sizeof(char));
                if(!token_buffer[j-1].value) exit(MEM_ALLOC_ERR_);
                memcpy(token_buffer[j-1].value, buffer + 1, strlen(buffer) - 2);
                token_buffer[j-1].value[strlen(buffer) - 1] = 0;
                token_buffer[j-1].type = DQUOTED;
            }
            else if(is_set_expr(buffer) && j == 0)
            {
                token_buffer[j++].value
                    = (char*)malloc((strlen(buffer) + 1) * sizeof(char));
                if(!token_buffer[j-1].value) exit(MEM_ALLOC_ERR_);
                strcpy(token_buffer[j-1].value, buffer);
                token_buffer[j-1].type = SET_EXPR;
            }
            else
            {
                token_buffer[j++].value
                    = (char*)malloc((strlen(buffer) + 1) * sizeof(char));
                if(!token_buffer[j-1].value) exit(MEM_ALLOC_ERR_);
                strcpy(token_buffer[j-1].value, buffer);
                token_buffer[j-1].type = UNQUOTED;
            }
        }
        i += strlen(raw_tokenized + i) + 1;
    }

    result = (Token**)malloc((j + 1) * sizeof(Token*));
    if(!result) exit(MEM_ALLOC_ERR_);
    for(i = 0; i < j; i++)
    {
        result[i] = (Token*)malloc(sizeof(Token));
        if(!result[i]) exit(MEM_ALLOC_ERR_);
        *(result[i]) = token_buffer[i];
    }
    result[i] = NULL;
    *tokenc = j;
    return result;
}
