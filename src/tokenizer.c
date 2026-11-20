#include "internal.h"

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

MatchResult tok_consume_match(Tokenizer* tok, char match, TokenType type)
{
    MatchResult res = {};

    if (tok->text.chars[tok->pos] == match)
    {
        res.matched = true;
        res.token = (Token){.type = type, .start_pos = tok->pos, .len = 1};
        tok->pos++;
    }
    return res;
}

int tok_consume(Tokenizer* tok, int len)
{
    int lenClamped = len;
    if (tok->pos + len >= tok->text.len)
    {
        int overshoot = (tok->pos + len) - tok->text.len;
        lenClamped = len - overshoot;
    }

    tok->pos += lenClamped;
    return lenClamped;
}

bool tok_lookahead(Tokenizer* tok, str matchList)
{
    int laPos = tok->pos + 1;
    if (laPos >= tok->text.len) return false;

    return str_matches_any(tok->text.chars[laPos], matchList);
}

bool tok_peek(Tokenizer* tok, str matchList)
{
    if (tok->pos >= tok->text.len) return false;

    return str_matches_any(tok->text.chars[tok->pos], matchList);
}

Token tok_next(Tokenizer* tok)
{
    if (tok->pos >= tok->text.len) return (Token){.type = TOK_EOF};

    SR(tok_consume_match(tok, '\n', TOK_NL));
    SR(tok_consume_match(tok, '#', TOK_HASH));
    SR(tok_consume_match(tok, '`', TOK_TICK));
    SR(tok_consume_match(tok, '*', TOK_STAR));
    SR(tok_consume_match(tok, '=', TOK_EQ));
    SR(tok_consume_match(tok, '[', TOK_BRAK));
    SR(tok_consume_match(tok, ']', TOK_BRAK));
    SR(tok_consume_match(tok, '~', TOK_STRIKE));
    SR(tok_consume_match(tok, '>', TOK_CARRET));

    Token t = {.type = TOK_TEXT, .start_pos = tok->pos};

    do
    {
        int actual = tok_consume(tok, 1);
        t.len += actual;

        // reached text end
        if (NOT(actual)) return t;
        // checking for end token
    } while (NOT(tok_peek(tok, str_static("\n"))));

    return t;
}
