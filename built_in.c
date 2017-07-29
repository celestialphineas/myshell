#include "built_in.h"

static const unsigned INIT_BUILT_IN_SIZE_SCALE = 6;

static int echo_built_in(int argc, char **argv);

HashMap *built_in_table;

static void push_function(const char *str, BuiltIn func)
{
    void **space = (void**)malloc(sizeof(void*));
    *space = (void*)func;
    insert_entry(built_in_table, str, space, sizeof(void*));
    free(space);
}

void init_built_in_table()
{
    if(!built_in_table)
        built_in_table
            = create_hash_map(INIT_BUILT_IN_SIZE_SCALE, BKDR_hash);
    if(!built_in_table) exit(BUILT_IN_HASH_ERR_);
    push_function("echo", echo_built_in);
    return;
}

int is_built_in(const char *str)
{
    if(!find_entry(built_in_table, str)) return false;
    else return true;
}

BuiltIn *get_built_in(const char *str)
{
    const HashEntry *found = find_entry(built_in_table, str);
    if(!found) return NULL;
    return found->value;
}

static int echo_built_in(int argc, char **argv)
{
    int i;
    if(!argv) return 0;
    for(i = 0; i < argc; i++)
    {
        printf("%s", argv[i]);
    }
    printf("\n");
    return 1;
}