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

int main(int argc, char **argv)
{
    // The initialization cannot be removed
    init();
    get_command();
    get_continued_command();
    return 0;
}