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
    TOK_NONE,
    TOK_EOF,
    TOK_TEXT,
    TOK_SPACE,
    TOK_HASH,
    TOK_NL,
    TOK_STAR,
    TOK_EQ,
    TOK_BRAK_S,
    TOK_BRAK_E,
    TOK_STRIKE,
    TOK_CARRET,
    TOK_QUOTE,
    TOK_TICK,
} TokenType;

typedef struct
{
    TokenType type;
    int start_pos;
    int len;
    int repeat_count;
} Token;

typedef struct
{
    char sym;
    int count;
} ColorType;

typedef struct
{
    str text;
    int pos;

    /*
     * Text or EOF = NONE
     * type that determines the current color
     */
    TokenType color_type;
    bool reset_after;

    // after text end -> till clear
    str before_end_col;

    // text to add after the token
    // -> To color token & text differently
    // & to hide text etc
    bool after_token_switch;
    str after_token_col;

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
