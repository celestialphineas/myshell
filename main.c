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
static void handle_args(int argc, char **argv);

int main(int argc, char **argv)
{
    handle_args(argc, argv);
    init();
    interactive_loop();
    return 0;
}

static void interactive_loop()
{
    while(1)
    {
        char *command = prompt1_read();
        char **separated = NULL;
        int cmdc;
        int found_background = 0;
        Job *job;

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

        if(separated) free(separated);
        separated = separate_commands(command, &cmdc);
        if(!separated)
        {
            print_myshell_err("Syntax error, no job is created.");
            continue;
        }
        job = command_to_job(separated[0], &found_background);
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
        clean_up_jobs();
    }
    return;
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
        printf("Cannot find file： %s\n", argv[option_count]);
        exit(2);
    }
    return;
}