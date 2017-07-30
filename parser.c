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

// Test if a token is a valid stdin/out/err file descriptor
static boolean is_std_fd(const Token *token)
{
    if(!token) return false;
    if(token->type != SQUOTED
    && token->type != DQUOTED
    && token->type != UNQUOTED)
        return false;
    if(strlen(token->value) != 1) return false;
    if(token->value[0] >= '0' && token->value[1] <= '2') return true;
    return false;
}

// Give the type of a control token
// IN_RD                <   input redirection
// OUT_RD               >   output redirection
// OUT_RD_APPEND        >>  output redirection (append)
// OUT_RD_DUP           >&  output redirection duplicated stdout/err
// OUT_RD_APPEND_DUP    >>& output redirection duplicated fd (append)
// PIPE_CONTROL         |   pipe
// PIPELINE_AND         &&  pipeline and (next if succeed)
// PIPELINE_OR          ||  pipeline or (next if fail)
// BG_CONTROL           &   background job indicator
// LEFT_BRACKET         (   left bracket
// RIGHT_BRACKET        )   right bracket
// DLEFT_BRACKET        ((  left double bracket (No use in myshell)
// DRIGHT_BRACKET       ))  right double bracket (No use in myshell)
typedef enum
{
    UNKNOWN = 0,
    IN_RD = 1, OUT_RD = 2, OUT_RD_APPEND = 3,
    OUT_RD_DUP = 4, OUT_RD_APPEND_DUP = 5,
    PIPE_CONTROL = 6, PIPELINE_AND = 7, PIPELINE_OR = 8,
    BG_CONTROL = 9, LEFT_BRACKET = 10, RIGHT_BRACKET = 11,
    DLEFT_BRACKET = 12, DRIGHT_BRACKET = 13
} ControlType;

static ControlType control_type(const Token *token)
{
    if(!token) return false;
    if(token->type != CONTROL) return false;
    if(!strcmp(token->value, "<"))      return IN_RD;
    if(!strcmp(token->value, ">"))      return OUT_RD;
    if(!strcmp(token->value, ">>"))     return OUT_RD_APPEND;
    if(!strcmp(token->value, ">&"))     return OUT_RD_DUP;
    if(!strcmp(token->value, ">>&"))    return OUT_RD_APPEND_DUP;
    if(!strcmp(token->value, "|"))      return PIPE_CONTROL;
    if(!strcmp(token->value, "&&"))     return PIPELINE_AND;
    if(!strcmp(token->value, "||"))     return PIPELINE_OR;
    if(!strcmp(token->value, "&"))      return BG_CONTROL;
    if(!strcmp(token->value, "("))      return LEFT_BRACKET;
    if(!strcmp(token->value, ")"))      return RIGHT_BRACKET;
    if(!strcmp(token->value, "(("))     return DLEFT_BRACKET;
    if(!strcmp(token->value, "))"))     return DRIGHT_BRACKET;
    return UNKNOWN;
}

// This function read in tokens,
// convert the input tokens to a single process in a manner
// And take out the used tokens
Process *read_in_process(Token ***ptokenv, int *ptokenc, int *found_bg)
{
    // Result
    Process *result = NULL;
    // Local tokenv and tokenc
    Token **tokenv;
    int tokenc;
    // used, for token clean up
    int i, j;
    int used[MAX_TOKEN_NUM] = {};
    // These will be passed to create_process
    int argc_ = 0;
    char *argv_buffer[MAX_TOKEN_NUM] = {};
    boolean is_pipe_ = 0;
    boolean append_out_ = 0;
    boolean append_err_ = 0;
    boolean pipeline_discipline_ = NORMAL;
    char *infile_ = NULL;
    char *outfile_ = NULL;
    char *errfile_ = NULL;
    int infile_fd_ = 0;
    int outfile_fd_ = 1;
    int errfile_fd_ = 2;

    // Test argument validity
    if(!ptokenv || !*ptokenv) return NULL;
    if(!ptokenc) return NULL;
    if(!found_bg) return NULL;
    // Set local
    tokenv = *ptokenv;
    tokenc = *ptokenc;

    // Handle the control tokens
    for(i = 0; i < tokenc; i++)
    {
        if(used[i]) continue;
        if(!tokenv[i]->value || !strcmp(tokenv[i]->value, "\n"))
        {
            used[i] = true;
            continue;
        }
        // |, terminates
        if(control_type(tokenv[i]) == PIPE_CONTROL)
        {
            used[i] = true;
            is_pipe_ = true;
            break;
        }
        // &, terminates
        if(control_type(tokenv[i]) == BG_CONTROL)
        {
            used[i] = true;
            *found_bg = 1;
            break;
        }
        // ||, terminates
        if(control_type(tokenv[i]) == PIPELINE_OR)
        {
            used[i] = true;
            pipeline_discipline_ = NEXT_IF_FAILURE;
            break;
        }
        // &&, terminates
        if(control_type(tokenv[i]) == PIPELINE_AND)
        {
            used[i] = true;
            pipeline_discipline_ = NEXT_IF_SUCCUSS;
            break;
        }
        // <
        if(control_type(tokenv[i]) == IN_RD)
        {
            used[i] = true;
            if(!tokenv[i + 1])
            {
                print_myshell_err("Syntax error: Unexpected new line. ");
                return NULL;
            }
            if(tokenv[i + 1]->type == CONTROL || tokenv[i + 1]->type == SET_EXPR)
            {
                char err_info[MAX_HOSTNAME_LEN] = {};
                strcpy(err_info, "Syntax error: Unexpected token: ");
                strcat(err_info, tokenv[i]->value);
                print_myshell_err(err_info);
                return NULL;
            }
            infile_ = tokenv[i + 1]->value;
            used[i + 1] = true;
            continue;
        }
        // >
        if(control_type(tokenv[i]) == OUT_RD)
        {
            // Redirect stderr flag
            int rd_stderr = 0;
            used[i] = true;
            if(!tokenv[i + 1])
            {
                print_myshell_err("Syntax error: Unexpected new line. ");
                return NULL;
            }
            if(tokenv[i + 1]->type == CONTROL || tokenv[i + 1]->type == SET_EXPR)
            {
                char err_info[MAX_HOSTNAME_LEN] = {};
                strcpy(err_info, "Syntax error: Unexpected token: ");
                strcat(err_info, tokenv[i]->value);
                print_myshell_err(err_info);
                return NULL;
            }
            if(i > 0)
            if(is_std_fd(tokenv[i - 1]) && tokenv[i - 1]->value[0] == '2')
            {
                used[i - 1] = 1;
                rd_stderr = 1;
            }
            if(rd_stderr)
                errfile_ = tokenv[i + 1]->value;
            else outfile_ = tokenv[i + 1]->value;
            used[i + 1] = true;
            continue;
        }
        // >>
        if(control_type(tokenv[i]) == OUT_RD_APPEND)
        {
            // Redirect stderr flag
            int rd_stderr = 0;
            used[i] = true;
            if(!tokenv[i + 1])
            {
                print_myshell_err("Syntax error: Unexpected new line. ");
                return NULL;
            }
            if(tokenv[i + 1]->type == CONTROL || tokenv[i + 1]->type == SET_EXPR)
            {
                char err_info[MAX_HOSTNAME_LEN] = {};
                strcpy(err_info, "Syntax error: Unexpected token: ");
                strcat(err_info, tokenv[i]->value);
                print_myshell_err(err_info);
                return NULL;
            }
            if(i > 0)
            if(is_std_fd(tokenv[i - 1]) && tokenv[i - 1]->value[0] == '2')
            {
                used[i - 1] = 1;
                rd_stderr = 1;
            }
            if(rd_stderr)
            {
                errfile_ = tokenv[i + 1]->value;
                append_err_ = 1;
            }
            else
            {
                outfile_ = tokenv[i + 1]->value;
                append_out_ = 1;
            }
            used[i + 1] = true;
            continue;
        }
        // >&
        if(control_type(tokenv[i]) == OUT_RD_DUP)
        {
            // Redirect stderr flag
            int rd_stderr = 0;
            used[i] = true;
            if(!tokenv[i + 1])
            {
                print_myshell_err("Syntax error: Unexpected new line. ");
                return NULL;
            }
            if(!is_std_fd(tokenv[i + 1]))
            {
                char err_info[MAX_HOSTNAME_LEN] = {};
                strcpy(err_info, "Syntax error: Unexpected token: ");
                strcat(err_info, tokenv[i]->value);
                print_myshell_err(err_info);
                return NULL;
            }
            if(i > 0)
            if(is_std_fd(tokenv[i - 1]))
            {
                used[i - 1] = true;
                if(tokenv[i - 1]->value[0] == '2') rd_stderr = 1;
            }
            if(rd_stderr)
            {
                errfile_fd_ = tokenv[i + 1]->value[0] - '0';
            }
            else
            {
                outfile_fd_ = tokenv[i + 1]->value[0] - '0';
            }
            used[i + 1] = true;
            continue;
        }
        // >>&
        if(control_type(tokenv[i]) == OUT_RD_APPEND_DUP)
        {
            // Redirect stderr flag
            int rd_stderr = 0;
            used[i] = true;
            if(!tokenv[i + 1])
            {
                print_myshell_err("Syntax error: Unexpected new line. ");
                return NULL;
            }
            if(!is_std_fd(tokenv[i + 1]))
            {
                char err_info[MAX_HOSTNAME_LEN] = {};
                strcpy(err_info, "Syntax error: Unexpected token: ");
                strcat(err_info, tokenv[i]->value);
                print_myshell_err(err_info);
                return NULL;
            }
            if(i > 0)
            if(is_std_fd(tokenv[i - 1]) && tokenv[i - 1]->value[0] == '2')
            {
                used[i - 1] = true;
                rd_stderr = 1;
            }
            if(rd_stderr)
            {
                append_err_ = 1;
                errfile_fd_ = tokenv[i + 1]->value[0] - '0';
            }
            else
            {
                append_err_ = 1;
                outfile_fd_ = tokenv[i + 1]->value[0] - '0';
            }
            used[i + 1] = true;
            continue;
        }
    }

    // argc, argv
    for(j = 0; j < tokenc; j++)
    {
        if(used[j]) break;
        argv_buffer[argc_++] = tokenv[j]->value;
        used[j] = true;
    }
    // Test if unexpected token occurs
    for(; j < i; j++)
    {
        if(!used[j]) // Then there are unused tokens in the process command
        {
            char err_info[MAX_HOSTNAME_LEN] = {};
            strcpy(err_info, "Syntax error: Unexpected token: ");
            strcat(err_info, tokenv[j]->value);
            print_myshell_err(err_info);
            return NULL;
        }
    }
    // Construct result process
    result = create_process(argc_, argv_buffer,
        is_pipe_, append_out_, append_err_, pipeline_discipline_,
        infile_, outfile_, errfile_, infile_fd_, outfile_fd_, errfile_fd_);
    if(!result)
        print_myshell_err("Failed to create process. ");

    // Token reconstruct
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
    return result;
}

Job *command_to_job(char *cmd, int *found_bg)
{
    Token **tokenv;
    Job *result;
    int tokenc;
    int i;
    if(!cmd) return NULL;
    if(!is_complete_command(cmd)) return NULL;
    if(!found_bg) return NULL;

    *found_bg = 0;

    tokenv = tokenize(cmd, &tokenc);
    if(!tokenv) return NULL;

    // Shell expansion
    for(i = 0; i < tokenc; i++)
    {
        if(tokenv[i]->type == DQUOTED || tokenv[i]->type == UNQUOTED)
        {
            char *tilde_expanded = tilde_expansion(tokenv[i]->value);
            char *var_expanded = var_expansion(tilde_expanded);
            char *escape_expanded = escape_expansion(var_expanded);
            if(!escape_expanded)
            {
                if(tilde_expanded) free(tilde_expanded);
                if(var_expanded) free(var_expanded);
                print_myshell_err("Token expansion error. ");
                return NULL;
            }
            free(tilde_expanded); free(var_expanded);
            free(tokenv[i]->value);
            tokenv[i]->value = escape_expanded;
        }
    }

    // Do the followings in a loop
    // Process *read_in_process(Token ***ptokenv, int *ptokenc)
    // Handle the tokens
    // If meet NULL process also returns
    {
        Process *process_head = NULL;
        Process *current_process = NULL;
        while(tokenc)
        {
            if(!handle_assignment_expr(&tokenv, &tokenc))
            {
                // If failed
                print_myshell_err("Error occurred when parsing the assignment expression. ");
                return NULL;
            }
            if(!tokenc) return NULL;
            if(tokenv[0]->type == CONTROL)
            {
                char err_info[MAX_HOSTNAME_LEN] = {};
                strcpy(err_info, "Syntax error: Unexpected token: ");
                strcat(err_info, tokenv[0]->value);
                print_myshell_err(err_info);
                return NULL;
            }
            if(!process_head)
            {
                process_head = read_in_process(&tokenv, &tokenc, found_bg);
                if(!process_head) return NULL;
                current_process = process_head;
            }
            else
            {
                current_process->next = read_in_process(&tokenv, &tokenc, found_bg);
                if(!current_process->next) return NULL;
                current_process = current_process->next;
            }
        }
        // Construct the job
        result = create_job_in_list(process_head, cmd);
    }
    
    return result;
}