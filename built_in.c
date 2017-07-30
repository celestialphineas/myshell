#include "built_in.h"
#include "global.h"
#include "job_control.h"
#include "message.h"

static const unsigned INIT_BUILT_IN_SIZE_SCALE = 6;

// Built-in functions
static int echo_(int argc, char **argv);
static int exit_(int argc, char **argv);
static int help_(int argc, char **argv);
static int cd_(int argc, char **argv);

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
    push_function("echo", echo_);
    push_function("exit", exit_);
    push_function("help", help_);
    push_function("cd", cd_);
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

static int echo_(int argc, char **argv)
{
    int i;
    if(!argv) return 0;
    for(i = 1; i < argc; i++)
    {
        printf("%s ", argv[i]);
    }
    printf("\n");
    return 0;
}

static int exit_(int argc, char **argv)
{
    if(argc > 2)
    {
        print_docs("exit");
        exit(2);
        return 0;
    }
    if(argc == 1)
    {
        exit(0);
    }
    else
    {
        int status = 2;
        sscanf(argv[1], "%d", &status);
        exit(status);
    }
    return 0;
}

static int help_(int argc, char **argv)
{
    if(argc <= 1 || !argv)
    {
        print_docs("help");
    }
    else
    {
        print_docs(argv[1]);
    }
    return 0;
}

static int cd_(int argc, char **argv)
{
    if(!argv || argc <= 0)
    {
        print_myshell_err("cd: argument error. ");
        return 1;
    }
    // Change directory to $HOME
    if(argc == 1)
    {
        chdir(getenv("HOME"));
        return 0;
    }
    else
    {
        if(chdir(argv[1]) == -1)
        {
            char err_info[MAX_HOSTNAME_LEN] = {};
            strcpy(err_info, "cd: ");
            strcat(err_info, argv[1]);
            strcat(err_info, ": No such directory.");
            print_myshell_err(err_info);
            return 1;
        }
        push_environ_to_var_table();
        return 0;
    }
}