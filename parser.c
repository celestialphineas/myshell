#include "parser.h"

boolean is_blank_char(char c)
{
    int i;
    char black_char[] = BLANK_CHAR;
    for(i = 0; i < strlen(black_char); i++)
    {
        if(c == black_char[i]) return true;
        else return false;
    }
    return false;
}