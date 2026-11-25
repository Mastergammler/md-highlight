#include "internal.h"

// short circuit behaviour on result type
#define _sc_(tokenFn)                                                          \
    do                                                                         \
    {                                                                          \
        MatchResult res = (tokenFn);                                           \
        if (res.matched) return res.token;                                     \
    } while (0)

static char SYMBOL_TABLE[] = {
    [TOK_NL] = '\n',    [TOK_STAR] = '*',   [TOK_EQ] = '=',
    [TOK_TICK] = '`',   [TOK_HASH] = '#',   [TOK_BOPEN] = '[',
    [TOK_BCLOSE] = ']', [TOK_STRIKE] = '~', [TOK_CARRET] = '>',
    [TOK_SPACE] = ' ',  [TOK_QUOTE] = '"',  [TOK_DASH] = '-',
    [TOK_DOT] = '.',

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
            res.token_pos =
                (TokenPosition){.token = {.type = type}, .start_pos = tok->pos};
        }

        res.token_pos.token.count++;
        res.token_pos.len++;
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

/*
 * Peeks the current char without changing the tokenizers position
 */
bool tok_peekc(Tokenizer* tok, char c)
{
    if (tok->pos >= tok->text.len) return false;
    return tok->text.chars[tok->pos] == c;
}

TokenType determine_tok_after(int num, Tokenizer* tok)
{
    int tokenPos = tok->pos + num;
    if (tokenPos >= tok->text.len) return TOK_NONE;

    char cur = tok->text.chars[tokenPos];
    for (int i = 0; i < LEN(SYMBOL_TABLE); i++)
    {
        if (cur == SYMBOL_TABLE[i]) return (TokenType)i;
    }

    return TOK_NONE;
}

TokenPosition tok_next(Tokenizer* tok)
{
    if (tok->pos >= tok->text.len)
        return (TokenPosition){.token = {.type = TOK_EOF}};

    TokenType specialType = determine_tok_after(0, tok);

    if (specialType > TOK_NONE)
    {
        TokenPosition tpos = {.token = {.type = specialType},
                              .start_pos = tok->pos};

        while (tok_peekc(tok, SYMBOL_TABLE[specialType]))
        {
            int consumed = tok_consume(tok, 1);
            tpos.len += consumed;
            tpos.token.count += consumed;
        }

        for (int i = 0; i < LEN(tpos.token.lookahead); i++)
        {
            TokenType laType = determine_tok_after(i, tok);

            if (laType > TOK_NONE)
                tpos.token.lookahead[i] = determine_tok_after(i, tok);
            else
                // FIXME: Unless we reached the end of the file
                tpos.token.lookahead[i] = TOK_TEXT;
        }

        return tpos;
    }

    TokenPosition tpos = {.token = {.type = TOK_TEXT}, .start_pos = tok->pos};

    do
    {
        int actual = tok_consume(tok, 1);
        tpos.len += actual;
        // we're counting the chars in the text also
        tpos.token.count += actual;

        // reached text end
        if (NOT(actual)) return tpos;

        // checking for section terminator coming up
        // FIXME: actually, this is incorrect, because a terminator might be
        // multiple symbols, and this text might just contain a single one or
        // something
        TokenType startToken = tok->highlight.trigger.type;
        TokenType endToken = tok->highlight.terminator.type;
        if (startToken > TOK_NONE && tok_peekc(tok, SYMBOL_TABLE[endToken]))
        {
            return tpos;
        }
        else
        {
            for (int i = TOK_SPACE + 1; i < LEN(SYMBOL_TABLE); i++)
            {
                // STFO: this goes through the whole table?
                // -> Is this good?
                // => Also peek checks multiple things etc
                if (tok_peekc(tok, SYMBOL_TABLE[i])) return tpos;
            }
        }

        // checking for end token
    } while (NOT(tok_peek(tok, str_static("\n"))));

    return tpos;
}
