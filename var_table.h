// Variable table
// ==============
// Author: Celestial Phineas @ ZJU
//         (Yehang YIN)
// This file implements a hash table for storing variables
// In bash, variables are also lists.
// myshell works in the same manner.
// All entries of the hash table are lists of strings.
// Each records elec, element count
// and elev, element value
// This design approach is very alike argc and argv
#ifndef CELPHI_VAR_TABLE_H
#define CELPHI_VAR_TABLE_H
#include "global.h"
#include "hash_map.h"

// It is very important to know that elev must work in a shallow way
// That is because in C, we don't have such "destructor" stuff
// It is impossible to destruct the hash table with the destructor
// of its dependency, the general HashMap (See hash_map.h)
typedef struct
{
    int elec;
    char **elev;
} Variable;

#define MIN_QUERY_OP    -2
typedef enum
{
    ElementCount = -1,
    ListAll = -2
} QueryOp;

extern Variable *EMPTY_VAR;

// This method initialize the variable table
void init_var_table();
// The update_variable method does not make a copy of var
// But it will free resources that it will override
// If you need to override NULL data,
// Pass EMPTY_VAR to the second slot
void update_variable(const char *key_, Variable *var_);
// The deletion is even lazier that it takes up spaces even after rehashing.
// The implementation of deleting a variable
// is the same to update with EMPTY_VAR
void delete_variable(const char *key_);
// The get_variable method returns a new string
// The return value is at least one character in size
// Don't forget to free it after using it.
char *get_variable(const char *key_, int index);

#endif