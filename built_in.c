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
static int pwd_(int argc, char **argv);
static int umask_(int argc, char **argv);
static int set_(int argc, char **argv);
static int unset_(int argc, char **argv);
static int shift_(int argc, char **argv);
static int jobs_(int argc, char **argv);
static int fg_(int argc, char **argv);
static int bg_(int argc, char **argv);

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
    push_function("pwd", pwd_);
    push_function("umask", umask_);
    push_function("set", set_);
    push_function("unset", unset_);
    push_function("shift", shift_);
    push_function("jobs", jobs_);
    push_function("fg", fg_);
    push_function("bg", bg_);
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

static int pwd_(int argc, char **argv)
{
    char buffer[MAX_PATH_LEN];
    if(!argv || argc <= 0)
    {
        print_myshell_err("pwd: argument error. ");
        return 1;
    }
    getcwd(buffer, MAX_COMMAND_LEN);
    printf("%s\n", buffer);
    return 0;
}

static int umask_(int argc, char **argv)
{
    mode_t mask = umask(0);
    umask(mask);
    if(!argv || argc <= 0)
    {
        print_myshell_err("umask: argument error. ");
        return 1;
    }
    if(argc >= 2)
    {
        sscanf(argv[1], "%u", &mask);
        umask(mask);
    }
    printf("%04u\n", mask);
    return 0;
}

static int set_(int argc, char **argv)
{
    int i;
    if(!argv || argc <= 0)
    {
        print_myshell_err("set: argument error. ");
        return 1;
    }
    if(argc == 1)
    {
        int status;
        pid_t forked = fork();
        if(!forked) execl("/usr/bin/env", "env", NULL);
        else if(forked == -1) exit(1);
        else wait(&status);
        puts("");
        return 1;
    }
    for(i = 1; i < argc; i++)
    {
        char i_string[4];

        // This converts the number to string
        sprintf(i_string, "%d", i);

        {
            Variable var;

            var.elec = 1;
            var.elev = (char**)malloc(2 * sizeof(char*));
            if(!var.elev) exit(MEM_ALLOC_ERR_);
            var.elev[1] = NULL;
            var.elev[0] = (char*)malloc((strlen(i_string) + 1) * sizeof(char));
            if(!var.elev[0]) exit(MEM_ALLOC_ERR_);
            strcpy(var.elev[0], argv[i]);
            update_variable(i_string, &var);
        }
    }
    {
        char i_string[4];
        sprintf(i_string, "%d", i);
        delete_variable(i_string);
    }
    return 0;
}

static int unset_(int argc, char **argv)
{
    int i;
    if(!argv || argc <= 0)
    {
        print_myshell_err("set: argument error. ");
        return 1;
    }
    for(i = 1; i < argc; i++)
    {
        delete_variable(argv[i]);
    }
    return 0;
}

static int shift_(int argc, char **argv)
{
    int i;
    if(!argv || argc <= 0)
    {
        print_myshell_err("cd: argument error. ");
        return 1;
    }
    for(i = 0; ; i++)
    {
        char i_string[4];
        char i_plus_1_string[4];
        // The i-th global argument value
        char *i_th_val;
        char *i_plus_1_th_val;
        
        // Convert the number to string
        sprintf(i_string, "%d", i);
        sprintf(i_plus_1_string, "%d", i + 1);

        // Get the variable values
        i_th_val = get_variable(i_string, 0);
        i_plus_1_th_val = get_variable(i_plus_1_string, 0);

        // Update variables, the last one
        if(!*i_plus_1_th_val)
        {
            delete_variable(i_string);
            free(i_th_val); free(i_plus_1_th_val);
            break;
        }
        // Update variables
        else
        {
            Variable var;
            var.elec = 1;
            var.elev = (char**)malloc(2 * sizeof(char*));
            if(!var.elev) exit(MEM_ALLOC_ERR_);
            var.elev[1] = NULL;
            var.elev[0] = i_plus_1_th_val;
            free(i_th_val);
            update_variable(i_string, &var);
        }
    }
    return 0;
}

static int jobs_(int argc, char **argv)
{
    extern Job *job_list;
    Job *j;
    
    if(!argv || argc <= 0)
    {
        print_myshell_err("jobs: argument error. ");
        return 1;
    }

    clean_up_jobs();
    for(j = job_list; j; j = j->next)
    {
        refresh_job_status(j);
        printf("[%d]", j->job_number);
        switch(j->state)
        {
            case UNSTARTED: printf("+\tUnstarted"); break;
            case COMPLETED: 
                printf("-\tCompleted");
                j->notified = true;
                break;
            case STOPPED: printf("+\tStopped"); break;
            case RUNNING: printf("-\tRunning"); break;
            default: break;
        }
        printf("\t\t%s\n", j->command);
    }

    return 0;
}

static int fg_(int argc, char **argv)
{
    int job_number = -1;
    Job *j;

    if(!argv || argc <= 0)
    {
        print_myshell_err("fg: argument error. ");
        return 1;
    }
    if(argc == 1)
    {
        print_myshell_err("fg: Input a job ID. ");
        return 1;
    }

    sscanf(argv[1], "%d", &job_number);

    j = find_job_by_id(job_number);
    if(!j)
    {
        char err_info[MAX_HOSTNAME_LEN];

        strcpy(err_info, "fg: ");
        strcat(err_info, argv[1]);
        strcat(err_info, ": No such job. ");
        print_myshell_err(err_info);
        return 1;
    }
    fg_job(j);
    return 0;
}

static int bg_(int argc, char **argv)
{
    int job_number = -1;
    Job *j;

    if(!argv || argc <= 0)
    {
        print_myshell_err("bg: argument error. ");
        return 1;
    }
    if(argc == 1)
    {
        print_myshell_err("bg: Input a job ID. ");
        return 1;
    }

    sscanf(argv[1], "%d", &job_number);

    j = find_job_by_id(job_number);
    if(!j)
    {
        char err_info[MAX_HOSTNAME_LEN];

        strcpy(err_info, "bg: ");
        strcat(err_info, argv[1]);
        strcat(err_info, ": No such job. ");
        print_myshell_err(err_info);
        return 1;
    }
    bg_job(j);
    return 0;
}