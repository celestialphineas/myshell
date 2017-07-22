#include "var_table.h"

static const unsigned INIT_HASH_SIZE_SCALE = 10;

HashMap *variable_table;
Variable *EMPTY_VAR;

void init_var_table()
{
    if(!EMPTY_VAR) EMPTY_VAR = (Variable*)malloc(sizeof(Variable));
    EMPTY_VAR->elec = 0;
    EMPTY_VAR->elev = NULL;
    if(!variable_table) variable_table = create_hash_map(INIT_HASH_SIZE_SCALE, BKDR_hash);
    if(!variable_table) exit(VAR_HASH_TABLE_ERR_);
    return;
}

Variable *destruct_var(Variable *var)
{
    if(!var) return NULL;
    if(var->elev)
    {
        int i;
        for(i = 0; i < var->elec; i++)
        {
            if(var->elev[i]) free(var->elev[i]);
            var->elev[i] = NULL;
        }
        free(var->elev);
    }
    return NULL;
}

void update_variable(const char *key_, Variable *var_)
{
    const HashEntry *found;
    // Empty key
    if(!key_ || !*key_) return;
    // Find key
    found = forced_find_entry(variable_table, key_);
    // If key not found
    if(!found)
    {
        insert_entry(variable_table, key_, var_, sizeof(Variable));
    }
    // Else the key is found
    else
    {
        Variable *found_variable = (Variable*)found->value;
        // If the entry is empty
        if(!found_variable)
        {
            insert_entry(variable_table, key_, var_, sizeof(Variable));
        }
        // If the entry is not empty
        else
        {
            // If the entry
            if(found_variable != EMPTY_VAR)
                found_variable = destruct_var(found_variable);
            insert_entry(variable_table, key_, var_, sizeof(Variable));
        }
    }
}

void delete_variable(const char *key_)
{
    update_variable(key_, EMPTY_VAR);
}

char *get_variable(const char *key_, int index)
{
    const HashEntry *found;
    char buffer[MAX_PROMPT_LEN];
    if(!key_ || index < MIN_QUERY_OP)
    {
        if(index == ListAll)
        {
            char *result = (char*)malloc(2*sizeof(char));
            if(!result) exit(MEM_ALLOC_ERR_);
            result[0] = '0';
            result[1] = 0;
            return result;
        }
        else
        {
            char *result = (char*)malloc(sizeof(char));
            if(!result) exit(MEM_ALLOC_ERR_);
            result[0] = 0;
            return result;
        }
    }
    buffer[0] = 0;
    found = find_entry(variable_table, key_);
    if(!found)
    {
        if(index == ElementCount)
        {
            char *result = (char*)malloc(2*sizeof(char));
            if(!result) exit(MEM_ALLOC_ERR_);
            result[0] = '0';
            result[1] = 0;
            return result;
        }
        else
        {
            char *result = (char*)malloc(sizeof(char));
            if(!result) exit(MEM_ALLOC_ERR_);
            result[0] = 0;
            return result;
        }
    }
    else
    {
        Variable *found_variable = (Variable*)found->value;
        if(index >= found_variable->elec || index < MIN_QUERY_OP)
        {
            char *result = (char*)malloc(sizeof(char));
            if(!result) exit(MEM_ALLOC_ERR_);
            result[0] = 0;
            return result;
        }
        else if(index == ListAll)
        {
            int i;
            char *result;
            for(i = 0; i < found_variable->elec; i++)
            {
                strcat(buffer, found_variable->elev[i]);
                if(i < found_variable->elec - 1)
                    strcat(buffer, " ");
            }
            result = (char*)malloc((strlen(buffer) + 1) * sizeof(char));
            if(!result) exit(MEM_ALLOC_ERR_);
            strcpy(result, buffer);
            return result;
        }
        else if(index == ElementCount)
        {
            char *result;
            sprintf(buffer, "%d", found_variable->elec);
            result = (char*)malloc((strlen(buffer) + 1) * sizeof(char));
            if(!result) exit(MEM_ALLOC_ERR_);
            strcpy(result, buffer);
            return result;
        }
        else
        {
            char *result;
            result
                = (char*)malloc((strlen(found_variable->elev[index]) + 1)
                * sizeof(char));
            if(!result) exit(MEM_ALLOC_ERR_);
            strcpy(result, found_variable->elev[0]);
            return result;
        }
    }
}