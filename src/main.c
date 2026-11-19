#include "internal.h"

ProgramArgs read_args(int argc, char** argv)
{
    ProgramArgs args = {};
    if (argc >= 2)
    {
        args.source_file = str_static(argv[1]);
        args.valid = true;
    }

    return args;
}

int main(int argc, char** argv)
{
    init_program(4096);
    ProgramArgs args = read_args(argc, argv);

    if (NOT(args.valid))
    {
        str_printc("Usage: <md-file>");
        return -1;
    }

    str content = file_read_all(args.source_file);
    str_print(content);

    return 0;
}
