#include "internal.h"

typedef enum
{
    TOK_SPACE,
    TOK_HASH,
    TOK_TEXT,
    TOK_NL,
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

// short circuit behaviour on result type
#define SR(tokenFn)                                                            \
    do                                                                         \
    {                                                                          \
        MatchResult res = (tokenFn);                                           \
        if (res.matched) return res.token;                                     \
    } while (0)

Tokenizer tok_init(str content)
{
    return (Tokenizer){.text = content};
}

MatchResult tok_match_single(Tokenizer* tok, char match, TokenType type)
{
    MatchResult res = {};

    if (tok->text.chars[tok->pos] == match)
    {
        res.matched = true;
        res.token = (Token){.type = type, .start_pos = tok->pos, .len = 1};
    }
    return res;
}

Token tok_next(Tokenizer* tok)
{
    if (tok->pos >= tok->text.len) return (Token){.type = TOK_EOF};

    char cur = tok->text.chars[tok->pos];

    SR(tok_match_single(tok, '\n', TOK_NL));
    SR(tok_match_single(tok, '#', TOK_HASH));

    // TODO: handle text token
    //  -> use lookahead, to check end state?
    //  -> and meanwhile just accumulate?
}
