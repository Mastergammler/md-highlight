#include "internal.h"

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
        default: return str_static("<<TODO>>");
    }
}
#define ANSI_RESET str_static("\033[0m")
#define ANSI_BOLD str_static("\033[1m")
#define ANSI_RED str_static("\033[31m")
#define ANSI_YELL str_static("\033[3;38;5;222m")
#define ANSI_YELL_BG str_static("\033[1;38;5;232;48;5;222m")
#define ANSI_BLUE str_static("\033[38;5;111m")
#define ANSI_CARRET str_static("\033[38;5;222;48;5;98m")
#define ANSI_STRIKE str_static("\033[9m")
#define ANSI_GRAY_ON_BLACK str_static("\033[38;5;248;48;5;233m")

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
    int maxIterations = 64;
    while (t.type != TOK_EOF && iterations++ < maxIterations)
    {
        t = tok_next(&tok);
        str dbgText = t.type != TOK_NL
                          ? str_sub(tok.text, t.start_pos, t.start_pos + t.len)
                          : str_static("<nl>");
        str text = str_sub(tok.text, t.start_pos, t.start_pos + t.len);

        switch (t.type)
        {
            case TOK_EOF:
            case TOK_NL: strb_add(ANSI_RESET, &strb, &Prog.dyn_mem); break;
            case TOK_HASH: strb_add(ANSI_RED, &strb, &Prog.dyn_mem); break;
            case TOK_STAR: strb_add(ANSI_YELL, &strb, &Prog.dyn_mem); break;
            case TOK_EQ: strb_add(ANSI_YELL_BG, &strb, &Prog.dyn_mem); break;
            case TOK_BRAK: strb_add(ANSI_BLUE, &strb, &Prog.dyn_mem); break;
            case TOK_STRIKE: strb_add(ANSI_STRIKE, &strb, &Prog.dyn_mem); break;
            case TOK_CARRET: strb_add(ANSI_CARRET, &strb, &Prog.dyn_mem); break;
            case TOK_TICK:
                strb_add(ANSI_GRAY_ON_BLACK, &strb, &Prog.dyn_mem);
                break;
            case TOK_TEXT:
            case TOK_SPACE:
            default:
                // do nothing, no color change
                break;
        }

        strb_add(text, &strb, &Prog.dyn_mem);

        // str_printc("Token: [%] %", fmt_s(token_type(t.type)), fmt_s(text));
    }

    str_print(strb_build(&strb));

    return 0;
}
