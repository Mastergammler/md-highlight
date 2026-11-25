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
    TOK_BOPEN,
    TOK_BCLOSE,
    TOK_STRIKE,
    TOK_CARRET,
    TOK_QUOTE,
    TOK_TICK,
    TOK_DASH,
    TOK_DOT,
    TOK_NUM
} TokenType;

typedef enum
{
    HL_NONE,
    HL_BLACK_ON_YELL,
    HL_BLUE,
    HL_GRAY,
    HL_DARK_GRAY,
    HL_GRAY_ON_BLACK,
    HL_GRAY_ON_BLACK_HIDE,
    HL_GREEN,
    HL_DARK_GREEN,
    HL_LGREEN_ON_DGREEN,
    HL_HIDE,
    HL_HIDE_STRONG,
    HL_LIGHT_RED,
    HL_LIGHTER_RED,
    HL_LIGHTEST_RED,
    HL_RED_BOLD,
    HL_RED,
    HL_STRIKE,
    HL_YELL_ON_PINK,
    HL_RED_ON_PINK,
    HL_RED_ON_RED_UNDER,
    HL_RED_ON_RED,
    HL_RED_UNDER,
    HL_YELL,
    HL_YELL_BOLD,
    HL_LIGHT_ON_PINK,
    HL_PURPLE,
    HL_DPURPLE,
    HL_YELL_ON_YELL,
    HL_count
} HighlightColor;

typedef enum
{
    SEC_NONE,
    /* = trigger */
    SEC_START,
    SEC_TEXT,
    /* = terminator */
    SEC_END,
    /* = after */
    SEC_RESET

} HighlightSection;

typedef struct
{
    TokenType type;
    int count;
    TokenType lookahead[4];
} Token;

typedef struct
{
    Token token;
    int start_pos;
    int len;
} TokenPosition;

typedef struct
{
    char sym;
    int count;
} ColorType;

typedef struct
{
    bool active;
    /* special case for additionally terminate on new line as well */
    bool nl_termination;

    Token trigger;
    Token terminator;

    HighlightColor start;
    HighlightColor text;
    HighlightColor term;

} Highlight;

typedef struct
{
    str text;
    int pos;

    Highlight highlight;
    HighlightSection cur_section;
    TokenPosition prev_token;
} Tokenizer;

typedef struct
{
    bool matched;
    TokenPosition token_pos;
} MatchResult;

typedef struct
{
    str* arr;
} StrBuilder;
