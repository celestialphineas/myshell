#include "message.h"

int main(int argc, char **argv)
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