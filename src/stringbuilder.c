#include "internal.h"

StrBuilder strb_init()
{
    return (StrBuilder){};
}

void strb_add(str text, StrBuilder* builder, Arena* arrMem)
{
    arr_append(builder->arr, text);
}

str strb_build(StrBuilder* builder)
{
    int strLen = 0;
    int builderLen = arr_len(builder->arr);
    for (int i = 0; i < builderLen; i++)
    {
        strLen += builder->arr[i].len;
    }

    // TODO: how did substring mode work exactly?
    StrPoolOptions opt = {.pool_idx = POOL_DISPLAY};
    char* start = pool_use(opt, strLen);

    char* next = start;
    for (int i = 0; i < builderLen; i++)
    {
        str cur = builder->arr[i];
        memcpy(next, cur.chars, cur.len);
        next += cur.len;
    }

    return (str){.chars = start, .len = strLen};
}
