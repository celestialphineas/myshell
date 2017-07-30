// myshell
// =======
// Author: Celestial Phineas @ ZJU
//         (Yehang YIN)
// Contact: yehangyin AT outlook DOT com
// Discription:
//      This is a simple implementation of a GNU/Linux or Unix shell
//      It is not of POSIX standard,
//      only a subset with some modifications is implemented.
//      It is an assignment of my summer course.

// File info
// =========
// Main program of myshell, handle the arguments passed
// For global definitions and initializations, see global.h for more info
#include "global.h"
#include "read_input.h"
#include "message.h"
#include "job_control.h"
#include "expansion.h"
#include "parser.h"

static void interactive_loop();
static void execute_script();
static void handle_args(int argc, char **argv);

int main(int argc, char **argv)
{
    handle_args(argc, argv);
    init();
    if(INTERACTIVE_MODE) interactive_loop();
    else execute_script();
    return 0;
}

static void handle_args(int argc, char **argv)
{
    // Option count read in the options
    // Supported options
    //  POSIX options
    //      -i          - Run in interactive mode
    //  GNU options
    //      --help      - Show help file
    //      --version   - Show version
    int option_count = 1;
    // Pass argc and argv to global namescope
    GLOBAL_ARGC = argc;
    GLOBAL_ARGV = argv;
    // If no argument is given
    if(option_count >= argc)
    {
        // Run in interactive mode
        // INTERACTIVE_MODE is a global variable defined in global.h
        INTERACTIVE_MODE = true;
        MYSHELL_ARG_OFFSET = 0;
        return;
    }
    // Play with the options
    for(option_count = 1; option_count < argc; option_count++)
    {
        // If this happens to be an option, i.e. starts with a '-'
        if(argv[option_count][0] == '-')
        {
            // There should be a trie or something like that here
            // However since there ain't much options,
            // I decided to be lazy and use simple control flows :P
            if(!strcmp(argv[option_count], "-i"))
            {
                INTERACTIVE_MODE = true;
            }
            else if(!strcmp(argv[option_count], "--help"))
            {
                // Show the myshell help
                print_docs("help");
                exit(SUCCESS_);
            }
            else if(!strcmp(argv[option_count], "--version"))
            {
                // Show the version
                print_docs("version");
                exit(SUCCESS_);
            }
            else
            {
                printf("myshell: %s: invalid option\n", argv[option_count]);
                print_docs("help");
                exit(MYSHELL_CALL_ERR_);
            }
        }
        else break;
    }
    // Handle the script file
    // If readable
    if(!access(argv[option_count], R_OK))
    {
        if(INTERACTIVE_MODE)
        {
            puts("myshell cannot run script in interactive mode.");
            exit(MYSHELL_CALL_ERR_);
        }
        else
        {
            // The argument offset is set option_count
            MYSHELL_ARG_OFFSET = option_count;
        }
    }
    else if(option_count < argc)
    {
        printf("Cannot find file: %s\n", argv[option_count]);
        exit(2);
    }
    return;
}

static void interactive_loop()
{
    while(1)
    {
        // Read a command with PS1
        char *command = prompt1_read();
        // Pointer to the separated command
        // Combined commands can be divided into several minors
        // each of which should be launched as a complete process pipeline
        char **separated = NULL;
        // Separated command count
        int cmdc;
        // Set 1 if found a command should be launched background
        int found_background = 0;
        // 
        int i;

        // myshell provides a feature to allow users to input a command 
        // in multiple lines
        // A function to test the completeness of a command is used here
        while(!is_complete_command(command))
        {
            char *cat = prompt2_read();
            char *temp_command
                = (char*)malloc((strlen(command) + strlen(cat) + 1) * sizeof(char));
            strcpy(temp_command, command);
            strcat(temp_command, cat);
            free(command);
            free(cat);
            command = temp_command;
            temp_command = remove_extra_blank(command);
            free(command);
            command = temp_command;
        }

        // See above, separated is a list of separated commands
        // See preprocessor.h for more information
        if(separated) free(separated);
        separated = separate_commands(command, &cmdc);
        if(!separated)
        {
            print_myshell_err("Syntax error, no job is created.");
            continue;
        }

        // Launch the separated commands in jobs
        for(i = 0; i < cmdc; i++)
        {
            Job *job = command_to_job(separated[i], &found_background);
            if(job)
            {
                if(found_background)
                {
                    launch_job(job, BACKGROUND);
                    printf("[%d]-\t%d\n", job->job_number, job->pgid);
                }
                else
                    launch_job(job, FORGROUND);
            }
        }
        clean_up_jobs();
    }
    return;
}

static void execute_script()
{
    char buffer[MAX_COMMAND_LEN];
    FILE *opened = fopen(GLOBAL_ARGV[MYSHELL_ARG_OFFSET], "r");

    if(!opened)
    {
        print_myshell_err("Script file load failure.");
        exit(1);
    }
    while(!feof(opened))
    {
        // Pointer to the separated command
        // Combined commands can be divided into several minors
        // each of which should be launched as a complete process pipeline
        char **separated = NULL;
        // Separated command count
        int cmdc;
        int i;

        fgets(buffer, MAX_COMMAND_LEN, opened);

        // myshell provides a feature to allow users to input a command 
        // in multiple lines
        // A function to test the completeness of a command is used here
        while(!is_complete_command(buffer))
        {
            if(feof(opened)) print_myshell_err("Unexpected end of the file. ");
            fgets(buffer + strlen(buffer),
                MAX_COMMAND_LEN - strlen(buffer), opened);
        }

        // See above, separated is a list of separated commands
        // See preprocessor.h for more information
        if(separated) free(separated);
        separated = separate_commands(buffer, &cmdc);
        if(!separated)
        {
            print_myshell_err("Syntax error, no job is created.");
            continue;
        }

        // Launch the separated commands in jobs
        for(i = 0; i < cmdc; i++)
        {
            int found_background;
            Job *job = command_to_job(separated[i], &found_background);
            launch_job(job, FORGROUND);
            clean_up_jobs();
        }
    }
    return;
}
