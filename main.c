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

static void handle_args(int argc, char **argv);

int main(int argc, char **argv)
{
    // Handle arguments before initialization
    handle_args(argc, argv);
    // The initialization cannot be removed
    init();
    prompt1_read();
    prompt2_read();
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
        printf("Cannot find file： %s\n", argv[option_count]);
        exit(2);
    }
    return;
}