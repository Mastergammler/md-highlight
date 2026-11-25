#include "internal.h"

// short circuit behaviour on result type
#define _sc_(tokenFn)                                                          \
    do                                                                         \
    {                                                                          \
        MatchResult res = (tokenFn);                                           \
        if (res.matched) return res.token;                                     \
    } while (0)

#define LEN(arr) (sizeof(arr) / sizeof(arr[0]))

static char SYMBOL_TABLE[] = {
    [TOK_NL] = '\n',    [TOK_STAR] = '*',   [TOK_EQ] = '=',
    [TOK_TICK] = '`',   [TOK_HASH] = '#',   [TOK_BRAK_S] = '[',
    [TOK_BRAK_E] = ']', [TOK_STRIKE] = '~', [TOK_CARRET] = '>',
    [TOK_QUOTE] = '"',

};

Tokenizer tok_init(str content)
{
    return (Tokenizer){.text = content};
}

MatchResult tok_consume_match(Tokenizer* tok, TokenType type)
{
    MatchResult res = {};

    while (tok->text.chars[tok->pos] == SYMBOL_TABLE[type])
    {
        if (NOT(res.matched))
        {
            res.matched = true;
            res.token = (Token){.type = type, .start_pos = tok->pos};
        }

        res.token.repeat_count++;
        res.token.len++;
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

bool tok_peekc(Tokenizer* tok, char c)
{
    if (tok->pos >= tok->text.len) return false;
    return tok->text.chars[tok->pos] == c;
}

/*
 * LOOKUP -> mapping
 *  => I can even iterate through that table then!
 * [TOK_TYPE] = {'*', 2}
 *    check via -> peek 2
 */

static ColorType TOKEN_TABLE[] = {
    [TOK_NL] = {.sym = '\n', .count = 1},
    [TOK_STAR] = {.sym = '*', .count = 1},
    [TOK_STAR] = {.sym = '*', .count = 2},
};

Token tok_next(Tokenizer* tok)
{
    if (tok->pos >= tok->text.len) return (Token){.type = TOK_EOF};

    _sc_(tok_consume_match(tok, TOK_NL));
    _sc_(tok_consume_match(tok, TOK_HASH));
    _sc_(tok_consume_match(tok, TOK_TICK));
    _sc_(tok_consume_match(tok, TOK_STAR));
    _sc_(tok_consume_match(tok, TOK_EQ));
    _sc_(tok_consume_match(tok, TOK_BRAK_S));
    _sc_(tok_consume_match(tok, TOK_BRAK_E));
    _sc_(tok_consume_match(tok, TOK_STRIKE));
    _sc_(tok_consume_match(tok, TOK_CARRET));
    _sc_(tok_consume_match(tok, TOK_QUOTE));

    Token t = {.type = TOK_TEXT, .start_pos = tok->pos};

    do
    {
        int actual = tok_consume(tok, 1);
        t.len += actual;

        // reached text end
        if (NOT(actual)) return t;

        // within a color && found the same token again
        if (tok->color_type > TOK_SPACE &&
            tok_peekc(tok, SYMBOL_TABLE[tok->color_type]))
        {
            return t;
        }
        else
        {
            for (int i = TOK_SPACE + 1; i < LEN(SYMBOL_TABLE); i++)
            {
                // STFO: this goes through the whole table?
                // -> Is this good?
                // => Also peek checks multiple things etc
                if (tok_peekc(tok, SYMBOL_TABLE[i])) return t;
            }
        }

        // checking for end token
    } while (NOT(tok_peek(tok, str_static("\n"))));

    return t;
}
