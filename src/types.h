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

typedef enum
{
    TOK_SPACE,
    TOK_HASH,
    TOK_TEXT,
    TOK_NL,
    TOK_STAR,
    TOK_EQ,
    TOK_BRAK,
    TOK_STRIKE,
    TOK_CARRET,
    TOK_TICK,
    TOK_EOF
} TokenType;

typedef struct
{
    TokenType type;
    int start_pos;
    int len;
} Token;

typedef struct
{
    str text;
    int pos;
} Tokenizer;

typedef struct
{
    bool matched;
    Token token;
} MatchResult;

typedef struct
{
    str* arr;
} StrBuilder;
