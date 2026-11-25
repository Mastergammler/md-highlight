#include "internal.h"

#define DEBUG false

#define ANSI_RESET str_static("\033[0m")
// I guess close enough for now
#define ANSI_HIDDEN_FG str_static("\033[38;5;232m")
#define ANSI_BOLD str_static("\033[1m")
#define ANSI_RED str_static("\033[4;38;5;203;48;5;52m")
#define ANSI_BOLD_RED str_static("\033[1;38;5;217m")
#define ANSI_YELL str_static("\033[3;38;5;222m")
#define ANSI_YELL_BG str_static("\033[1;38;5;232;48;5;222m")
#define ANSI_YELL_HIDE str_static("\033[1;38;5;222;48;5;222m")
#define ANSI_BLUE str_static("\033[38;5;111m")
#define ANSI_GREEN str_static("\033[38;5;148m")
#define ANSI_CARRET str_static("\033[38;5;222;48;5;98m")
#define ANSI_STRIKE str_static("\033[9m")
#define ANSI_EOL str_static("\033[80G\033[1K\033[1G")
#define ANSI_FULL_LINE str_static("\033[K")
#define ANSI_GRAY_ON_BLACK str_static("\033[38;5;248;48;5;233m")

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
        default: return str_static("<<TODO>>");
    }
}

void color_reset(Tokenizer* tok, StrBuilder* strb)
{
    strb_add(ANSI_RESET, strb, &Prog.dyn_mem);
    tok->color_type = TOK_NONE;
}

bool matches(int count, TokenType type, Token cur)
{
    return cur.type == type && cur.repeat_count == count;
}

bool is_end_of(TokenType type, Token cur, Tokenizer t)
{
    return cur.type == type && t.color_type == type;
}

void color_set(Tokenizer* tok, StrBuilder* strb, Token cur)
{
    if (cur.type == TOK_EOF || cur.type == TOK_NL)
    {
        color_reset(tok, strb);
    }
    // skip cases
    else if (cur.type == TOK_TEXT || cur.type == TOK_SPACE)
    {
        // strb_add(ANSI_HIDDEN_FG, strb, &Prog.dyn_mem);
        //  do nothing
    }
    else if (cur.type == tok->color_type)
    {
        // prev type == current type => means end condition
        // -> Just skip special cases before!
        tok->reset_after = true;
    }

    // ------------- matching list starting ----------------
    // => actually turn this into a lookup of a semtable

    else if (matches(1, TOK_STAR, cur))
    {
        strb_add(ANSI_HIDDEN_FG, strb, &Prog.dyn_mem);
        tok->before_end_col = ANSI_HIDDEN_FG;
        tok->color_type = TOK_STAR;
        tok->after_token_switch = true;
        tok->after_token_col = ANSI_YELL;
    }
    else if (matches(2, TOK_STAR, cur))
    {
        strb_add(ANSI_HIDDEN_FG, strb, &Prog.dyn_mem);
        tok->before_end_col = ANSI_HIDDEN_FG;
        tok->color_type = TOK_STAR;
        tok->after_token_switch = true;
        tok->after_token_col = ANSI_BOLD_RED;
    }
    else if (matches(1, TOK_TICK, cur))
    {
        strb_add(ANSI_HIDDEN_FG, strb, &Prog.dyn_mem);
        tok->before_end_col = ANSI_HIDDEN_FG;
        tok->color_type = TOK_TICK;
        tok->after_token_switch = true;
        tok->after_token_col = ANSI_GRAY_ON_BLACK;
    }
    else if (matches(2, TOK_EQ, cur))
    {
        strb_add(ANSI_YELL_HIDE, strb, &Prog.dyn_mem);
        tok->before_end_col = ANSI_YELL_HIDE;
        tok->color_type = TOK_EQ;
        tok->after_token_switch = true;
        tok->after_token_col = ANSI_YELL_BG;
    }
    else if (matches(2, TOK_BRAK_S, cur))
    {
        strb_add(ANSI_BLUE, strb, &Prog.dyn_mem);
        tok->before_end_col = ANSI_BLUE;
        tok->color_type = TOK_BRAK_S;
        tok->after_token_switch = true;
        tok->after_token_col = ANSI_BLUE;
    }
}

// STFO: Can i just color the symbols with the bg color or something to hide
//  them, without actually having to remove them?
int main(int argc, char** argv)
{
    init_program(8196);
    ProgramArgs args = read_args(argc, argv);

    if (NOT(args.valid))
    {
        str_printc("Usage: <md-file>");
        return -1;
    }

    str content = file_read_all(args.source_file);
    Tokenizer tok = tok_init(content);

    StrBuilder strb = strb_init();

    Token t = {};
    int iterations = 0;
    int maxIterations = 80;

    while (t.type != TOK_EOF && iterations++ < maxIterations)
    {
        t = tok_next(&tok);
        str dbgText = t.type != TOK_NL
                          ? str_sub(tok.text, t.start_pos, t.start_pos + t.len)
                          : str_static("<nl>");
        str text = str_sub(tok.text, t.start_pos, t.start_pos + t.len);

        // TODO: implement specific reset events per section
        // -> every section has their own one

        color_set(&tok, &strb, t);

        switch (t.type)
        {
            case TOK_HASH:
                strb_add(ANSI_RED, &strb, &Prog.dyn_mem);
                strb_add(ANSI_FULL_LINE, &strb, &Prog.dyn_mem);
                break;
            case TOK_QUOTE: strb_add(ANSI_GREEN, &strb, &Prog.dyn_mem); break;
            case TOK_STRIKE: strb_add(ANSI_STRIKE, &strb, &Prog.dyn_mem); break;
            case TOK_CARRET:
                strb_add(ANSI_CARRET, &strb, &Prog.dyn_mem);
                strb_add(ANSI_EOL, &strb, &Prog.dyn_mem);
                break;
            default:
                // do nothing, no color change
                break;
        }

        if (tok.reset_after)
        {
            strb_add(tok.before_end_col, &strb, &Prog.dyn_mem);
        }

        strb_add(text, &strb, &Prog.dyn_mem);

        if (tok.reset_after)
        {
            color_reset(&tok, &strb);
            tok.reset_after = false;
            tok.before_end_col = (str){};
        }
        else if (tok.after_token_switch)
        {
            strb_add(tok.after_token_col, &strb, &Prog.dyn_mem);
            tok.after_token_switch = false;
            tok.after_token_col = (str){};
        }

        if (DEBUG)
            str_printc("Token: [%|%] %", fmt_s(token_type(t.type)),
                       fmt_n(t.repeat_count), fmt_s(dbgText));
    }

    str_print(strb_build(&strb));

    return 0;
}
