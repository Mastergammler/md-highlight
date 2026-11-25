#include "colors.h"
#include "internal.h"

#define DEBUG false

ProgramArgs read_args(int argc, char** argv)
{
    ProgramArgs args = {};
    if (argc >= 2)
    {
        args.source_file = str_static(argv[1]);
        args.valid = true;
    }

    return args;
}

str token_type(TokenType type)
{
    switch (type)
    {
        case TOK_SPACE: return str_static("SPACE");
        case TOK_HASH: return str_static("HASH");
        case TOK_TEXT: return str_static("TEXT");
        case TOK_NL: return str_static("NL");
        case TOK_EOF: return str_static("EOF");
        case TOK_EQ: return str_static("EQ");
        case TOK_STAR: return str_static("STAR");
        case TOK_TICK: return str_static("TICK");
        case TOK_DASH: return str_static("DASH");
        case TOK_BOPEN: return str_static("BOPEN");
        case TOK_BCLOSE: return str_static("BCLOSE");
        default: return str_static("<<TODO>>");
    }
}

#define ANSI_RESET str_static("\033[0m")

void highlight_reset(Tokenizer* tok, StrBuilder* strb)
{
    strb_add(ANSI_RESET, strb, &Prog.dyn_mem);
    tok->highlight = (Highlight){};
    tok->cur_section = SEC_NONE;
}

bool matches(int count, TokenType type, Token cur)
{
    return cur.type == type && cur.count == count;
}

bool matches_wla(int count, TokenType curToken, Token hlToken, Token cur)
{

    if (DEBUG)
        str_printc("Check LA match for hl: [%]::[%,%,%,%] == [%]::[%,%,%,%]",
                   fmt_n(hlToken.type), fmt_n(hlToken.lookahead[0]),
                   fmt_n(hlToken.lookahead[1]), fmt_n(hlToken.lookahead[2]),
                   fmt_n(hlToken.lookahead[3]), fmt_n(cur.type),
                   fmt_n(cur.lookahead[0]), fmt_n(cur.lookahead[1]),
                   fmt_n(cur.lookahead[2]), fmt_n(cur.lookahead[3]));

    bool startTokenMatch = cur.type == curToken && cur.count == count;
    bool laMatch = true;
    for (int i = 0; i < LEN(hlToken.lookahead); i++)
    {
        if (hlToken.lookahead[i] == TOK_NONE) break;
        laMatch = laMatch && (hlToken.lookahead[i] == cur.lookahead[i]);
    }

    return startTokenMatch && laMatch;
}

/*
 * The section advancement really becomes a state machine atm
 * -> It might even become more complex
 */
void section_advance(Tokenizer* tok, Token cur)
{
    // if we're already at the last one, we have to wait for an actual reset
    if (tok->cur_section == SEC_RESET) return;

    if (tok->cur_section == SEC_START &&
        tok->highlight.terminator.type == cur.type)
    {
        tok->cur_section = SEC_END;
        return;
    }

    if (tok->cur_section == SEC_TEXT && tok->highlight.nl_termination &&
        cur.type == TOK_NL)
    {
        tok->cur_section = SEC_RESET;
        return;
    }

    if (tok->cur_section == SEC_TEXT &&
        tok->highlight.terminator.type != cur.type)
        return;

    // this is usually the NL case, because we need to reset before new line
    // that it works correctly, else it messes up ansi coloring
    if (tok->cur_section == SEC_TEXT && tok->highlight.term == HL_NONE &&
        tok->highlight.terminator.type == cur.type)
    {
        tok->cur_section = SEC_RESET;
    }
    else
    {
        tok->cur_section++;
    }
}

void add_section_color(Tokenizer* tok, StrBuilder* strb)
{
    switch (tok->cur_section)
    {
        case SEC_NONE: break;
        case SEC_START:
            strb_add(COLOR_TABLE[tok->highlight.start], strb, &Prog.dyn_mem);
            break;
        case SEC_TEXT:
            strb_add(COLOR_TABLE[tok->highlight.text], strb, &Prog.dyn_mem);
            break;
        case SEC_END:
            strb_add(COLOR_TABLE[tok->highlight.term], strb, &Prog.dyn_mem);
            break;
        case SEC_RESET: highlight_reset(tok, strb); break;
        default:
            str_printc("ERR: Section was: %", fmt_n(tok->cur_section));
            break;
    }
}

void color_set(Tokenizer* tok, StrBuilder* strb, Token cur)
{
    if (cur.type == TOK_EOF)
    {
        highlight_reset(tok, strb);
    }
    else if (tok->cur_section > SEC_NONE)
    {
        section_advance(tok, cur);
        add_section_color(tok, strb);
    }

    /*
     * check if a new section is starting
     * This is disconnected from the if above because we might just
     * have ended a token, and starting the new one at the same time
     */
    if (tok->cur_section == SEC_NONE)
    {
        for (int i = 0; i < LEN(HIGHLIGHTS); i++)
        {
            Highlight hl = HIGHLIGHTS[i];
            if (hl.trigger.lookahead[0] > TOK_NONE)
            {

                if (matches_wla(hl.trigger.count, hl.trigger.type, hl.trigger,
                                cur))
                {
                    tok->highlight = hl;
                    tok->cur_section = SEC_START;
                    break;
                }
            }
            else if (matches(hl.trigger.count, hl.trigger.type, cur))
            {
                tok->highlight = hl;
                tok->cur_section = SEC_START;
                break;
            }
        }
    }

    add_section_color(tok, strb);

    /*
       break; case TOK_CARRET: strb_add(ANSI_CARRET, &strb, &Prog.dyn_mem);
                    strb_add(ANSI_EOL, &strb, &Prog.dyn_mem);
                    break;

    //    else if (matches_wla(1, TOK_HASH, TOK_SPACE, cur))
    {
        strb_add(ANSI_RED_BG_UNDER, strb, &Prog.dyn_mem);
        strb_add(ANSI_FULL_LINE, strb, &Prog.dyn_mem);
        tok->color_type = TOK_HASH;
    }
    */
}

// STFO: Can i just color the symbols with the bg color or something to hide
//  them, without actually having to remove them?
int main(int argc, char** argv)
{
    init_program(32 * 4048);
    init_color_table();
    ProgramArgs args = read_args(argc, argv);

    if (NOT(args.valid))
    {
        str_printc("Usage: <md-file>");
        return -1;
    }

    str content = file_read_all(args.source_file);

    Timer timer = {};
    timer_start(&timer);

    Tokenizer tok = tok_init(content);
    StrBuilder strb = strb_init();

    TokenPosition curTpos = {};
    int iterations = 0;

    while (curTpos.token.type != TOK_EOF)
    {
        curTpos = tok_next(&tok);
        str text = str_sub(tok.text, curTpos.start_pos,
                           curTpos.start_pos + curTpos.len);

        color_set(&tok, &strb, curTpos.token);
        strb_add(text, &strb, &Prog.dyn_mem);

        tok.prev_token = curTpos;

        if (DEBUG)
        {
            str dbgText = curTpos.token.type != TOK_NL
                              ? str_sub(tok.text, curTpos.start_pos,
                                        curTpos.start_pos + curTpos.len)
                              : str_static("<nl>");
            str_printc("Token: [%|%] '%' :: LA [%,%,%,%]",
                       fmt_s(token_type(curTpos.token.type)),
                       fmt_n(curTpos.token.count), fmt_s(dbgText),
                       fmt_n(curTpos.token.lookahead[0]),
                       fmt_n(curTpos.token.lookahead[1]),
                       fmt_n(curTpos.token.lookahead[2]),
                       fmt_n(curTpos.token.lookahead[3]));
        }
    }

    float elapsed = timer_elapsed_ms(&timer);

    // FIXME: CRASH - when the text contains a % it tries to find a formatter
    //  -> But there is ofc none there, how can I fix this?
    str_print(strb_build(&strb));
    str_printc("| % ms Parsing | File size: %", fmt_f(elapsed, 3),
               fmt_n(content.len));

    return 0;
}
