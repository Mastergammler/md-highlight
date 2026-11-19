#include <string/module.h>

typedef struct
{
    bool valid;
    str source_file;
} ProgramArgs;

typedef struct
{
    bool open;
    FILE* stream;
    int len;
    str path;
} FsRes;
