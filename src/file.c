#include "internal.h"

FsRes file_open(str path)
{
    FsRes res = {.path = path};
    res.stream = fopen(path.chars, "r");

    if (res.stream)
    {
        fseek(res.stream, 0, SEEK_END);
        res.len = ftell(res.stream);
        res.open = true;

        fseek(res.stream, 0, SEEK_SET);
    }

    return res;
}

str file_read_all(str path)
{
    FILE* file = fopen(path.chars, "r");

    if (!file)
    {
        // TODO: some ansi coloring would be nice
        str_printc("[ERR] Could not open file '%'", fmt_s(path));
        return (str){};
    }

    StrPoolOptions opt = {.pool_idx = POOL_DISPLAY};
    str_pool_reset(opt);

    fseek(file, 0, SEEK_END);
    int fileLen = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* srcPtr = pool_use(opt, fileLen);
    str content = {.chars = srcPtr, .len = fileLen};
    fread(srcPtr, 1, fileLen, file);
    fclose(file);

    return content;
}

void file_close(FsRes res)
{
    // fclose is NOT null safe
    if (res.stream) fclose(res.stream);
}
