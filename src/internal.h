#ifndef SMATCH_INTERNAL
#define SMATCH_INTERNAL

#include "types.h"

#include <alloc/module.h>
#include <string/macros.h>
#include <string/module.h>

#define NOT(expr) !(expr)

typedef enum
{
    POOL_DEFAULT,
    POOL_PRINT,
    POOL_DISPLAY,
    POOL_count
} StrPools;

typedef struct
{
    // Timer timer;
    Arena mem;
    Arena dyn_mem;
    AllocContext mem_ctx;
    StringMemory strings;

} Program;

extern Program Prog;

void init_program(uint64_t memory);

FsRes file_open(str path);
str file_read_all(str path);
void file_close(FsRes res);

Tokenizer tok_init(str content);
Token tok_next(Tokenizer* tok);

StrBuilder strb_init();
void strb_add(str text, StrBuilder* builder, Arena* arrMem);
str strb_build(StrBuilder* builder);

#endif
