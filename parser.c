#include "parser.h"

// Return status: 0 for failure, 1 for success
boolean handle_assignment_expr(Token ***ptokenv, int *ptokenc)
{
    // Local tokenv and tokenc
    Token **tokenv;
    int tokenc;
    int used[MAX_TOKEN_NUM] = {};
    
    if(!ptokenv || !*ptokenv) return false;
    if(!ptokenc) return false;

    // Assign tokenv and tokenc
    tokenv = *ptokenv;
    tokenc = *ptokenc;

    // Handle set expr
    if(tokenv[0]->type == SET_EXPR)
    {
        // Find out the equal position
        int equal_pos = 0;
        for(equal_pos = 0; tokenv[0]->value[equal_pos]; equal_pos++)
        {
            if(tokenv[0]->value[equal_pos] == '=') break;
        }
        if(!tokenv[0]->value[equal_pos])
        {
            // Then an error occurs
            char err_info[MAX_HOSTNAME_LEN] = {};
            strcat(err_info, "Internal mistake: ");
            strcat(err_info, tokenv[0]->value);
            strcat(err_info, " is not an assignment expression. ");
            print_myshell_err(err_info);
            return false;
        }

        // [identifier]=[string]
        if(equal_pos != strlen(tokenv[0]->value) - 1)
        {
            char *identifier = (char*)malloc((equal_pos + 1) * sizeof(char));
            char *str
                = (char*)malloc((strlen(tokenv[0]->value) - equal_pos) * sizeof(char));
            // See var_table.h for definition
            Variable *var = (Variable*)malloc(sizeof(Variable));

            if(!identifier || !str || !var) exit(MEM_ALLOC_ERR_);
            strncpy(identifier, tokenv[0]->value, equal_pos);
            strcpy(str, tokenv[0]->value + equal_pos + 1);
            var->elec = 1;
            var->elev = (char**)malloc(2 * sizeof(char));
            if(!var->elev) exit(MEM_ALLOC_ERR_);
            var->elev[0] = str;
            var->elev[1] = NULL;
            // Push hash table
            update_variable(identifier, var);
            // To delete identifier is a must
            // but there is no need to delete str
            // See update_variable definition in var_table.h
            free(identifier);
            // Mark the token is used, in order that we can remove it later
            used[0] = 1;
        }
        // [identifier]= ...next token
        else
        {
            // [identifier]=(
            if(tokenv[1]->type == CONTROL && tokenv[1]->value[0] == '(')
            {
                int right_bracket_pos;
                int i;
                for(right_bracket_pos = 2;
                    right_bracket_pos < tokenc;
                    right_bracket_pos++)
                {
                    if(tokenv[right_bracket_pos]->type == CONTROL 
                        && tokenv[right_bracket_pos]->value[0] == ')')
                            break;
                }
                // An error occurs: Unpaired brackets
                if(tokenv[right_bracket_pos]->value[0] != ')')
                {
                    print_myshell_err("Syntax error: Unpaired bracket (. ");
                    return false;
                }
                used[0] = 1; used[1] = 1; used[right_bracket_pos] = 1;
                // Push the elements into the hash table
                if(right_bracket_pos - 1 > 1)
                {
                    Variable *var = (Variable*)malloc(sizeof(Variable));
                    if(!var) exit(MEM_ALLOC_ERR_);
                    var->elec = right_bracket_pos - 2;
                    var->elev = (char**)malloc((var->elec + 1) * sizeof(char*));
                    if(!var->elev) exit(MEM_ALLOC_ERR_);
                    var->elev[var->elec] = NULL;
                    for(i = 2; i < right_bracket_pos; i++)
                    {
                        var->elev[i - 2]
                            = (char*)malloc((strlen(tokenv[i]->value) + 1) * sizeof(char*));
                        if(!var->elev[i - 2]) exit(MEM_ALLOC_ERR_);
                        strcpy(var->elev[i - 2], tokenv[i]->value);
                        used[i] = 1;
                    }
                    // Finished constructing var, push it into the hash table
                    {
                        char buffer[MAX_HOSTNAME_LEN] = {};
                        strncpy(buffer, tokenv[0]->value, equal_pos);
                        update_variable(buffer, var);
                    }
                }
                else    // 0 element
                {
                    char buffer[MAX_HOSTNAME_LEN] = {};
                    strncpy(buffer, tokenv[0]->value, equal_pos);
                    delete_variable(buffer);
                }
            }
            // [identifier]= ...next token
            // In this case, we simply update the variable with an empty string
            else
            {
                char *identifier
                    = (char*)malloc(strlen(tokenv[0]->value) * sizeof(char));
                if(!identifier) exit(MEM_ALLOC_ERR_);
                strncpy(identifier, tokenv[0]->value, equal_pos - 1);
                // Update hash table
                delete_variable(identifier);
                free(identifier);
                used[0] = 1;
            }
        }

        // Don't forget to restore the arguments
        {
            Token *tokenv_buffer[MAX_TOKEN_NUM] = {};
            int temp_tokenc = 0;
            int i;
            for(i = 0; i < tokenc; i++)
            {
                if(!used[i] && tokenv[i]->value)
                    tokenv_buffer[temp_tokenc++] = tokenv[i];
                else tokenv[i] = destruct_token(tokenv[i]);
            }
            free(tokenv);
            tokenv = (Token**)malloc((temp_tokenc + 1) * sizeof(Token*));
            if(!tokenv) exit(MEM_ALLOC_ERR_);
            tokenv[temp_tokenc] = NULL;
            memcpy(tokenv, tokenv_buffer, temp_tokenc * sizeof(Token*));
            tokenc = temp_tokenc;

            *ptokenv = tokenv;
            *ptokenc = tokenc;
        }
    }
    // Thus there's no need to judge in the function "command_to_job"
    return true;
}

Job *command_to_job(char *cmd)
{
    Token **tokenv;
    int tokenc;
    if(!cmd) return NULL;
    if(!is_complete_command(cmd)) return NULL;

    tokenv = tokenize(cmd, &tokenc);
    if(!tokenv) return NULL;

    // Todo: shell expansions

    // Handle the tokens
    if(!handle_assignment_expr(&tokenv, &tokenc))
    {
        // If failed
        print_myshell_err("Error occurred when parsing the assignment expression. ");
    }

    // Todo: handle the pipes
    // Process *handle_pipes(Token ***ptokenv, int *ptokenc)
    // Todo: handle the redirections
    // Process *handle_redirections(Token ***ptokenv, int *ptokenc)
    return NULL;
}