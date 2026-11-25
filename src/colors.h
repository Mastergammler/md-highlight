#include "internal.h"

static str COLOR_TABLE[HL_count];

// I guess close enough for now
#define ANSI_HIDDEN_FG str_static("\033[38;5;232m")
#define ANSI_BOLD str_static("\033[1m")
#define ANSI_EOL str_static("\033[80G\033[1K\033[1G")
#define ANSI_FULL_LINE str_static("\033[K")

/*
 * Because i need to create my string types I can't put them into the table
 * directly, because they would need to be compile time constants
 */
static void init_color_table()
{
    COLOR_TABLE[HL_NONE] = (str){};
    COLOR_TABLE[HL_HIDE] = str_static("\033[38;5;244m");
    COLOR_TABLE[HL_HIDE_STRONG] = str_static("\033[38;5;238m");
    COLOR_TABLE[HL_YELL] = str_static("\033[3;38;5;222m");
    COLOR_TABLE[HL_YELL_BOLD] = str_static("\033[1;38;5;111m");
    COLOR_TABLE[HL_RED_BOLD] = str_static("\033[1;38;5;217m");
    COLOR_TABLE[HL_GREEN] = str_static("\033[38;5;149m");
    COLOR_TABLE[HL_DARK_GREEN] = str_static("\033[0;38;5;071m");
    COLOR_TABLE[HL_LGREEN_ON_DGREEN] = str_static("\033[38;5;71;48;5;023m");
    COLOR_TABLE[HL_BLUE] = str_static("\033[38;5;111m");
    COLOR_TABLE[HL_BLACK_ON_YELL] = str_static("\033[1;38;5;232;48;5;222m");
    COLOR_TABLE[HL_YELL_ON_YELL] = str_static("\033[1;38;5;222;48;5;222m");
    COLOR_TABLE[HL_STRIKE] = str_static("\033[9m");
    COLOR_TABLE[HL_GRAY_ON_BLACK] = str_static("\033[38;5;248;48;5;235m");
    COLOR_TABLE[HL_GRAY_ON_BLACK_HIDE] = str_static("\033[38;5;242;48;5;235m");
    COLOR_TABLE[HL_LIGHT_RED] = str_static("\033[38;5;210m");
    COLOR_TABLE[HL_LIGHTER_RED] = str_static("\033[38;5;217m");
    COLOR_TABLE[HL_LIGHTEST_RED] = str_static("\033[38;5;224m");
    COLOR_TABLE[HL_RED] = str_static("\033[38;5;203m");
    COLOR_TABLE[HL_GRAY] = str_static("\033[38;5;248m");
    COLOR_TABLE[HL_DARK_GRAY] = str_static("\033[38;5;243m");
    COLOR_TABLE[HL_YELL_ON_PINK] = str_static("\033[38;5;222;48;5;98m");
    COLOR_TABLE[HL_RED_ON_PINK] = str_static("\033[38;5;196;48;5;98m");
    COLOR_TABLE[HL_RED_ON_RED_UNDER] = str_static("\033[4;38;5;203;48;5;52m");
    COLOR_TABLE[HL_RED_ON_RED] = str_static("\033[38;5;203;48;5;52m");
    COLOR_TABLE[HL_RED_UNDER] = str_static("\033[4;38;5;203m");
    COLOR_TABLE[HL_LIGHT_ON_PINK] = str_static("\033[1;38;5;225m");
    COLOR_TABLE[HL_PURPLE] = str_static("\033[0;38;5;182m");
    COLOR_TABLE[HL_DPURPLE] = str_static("\033[1;38;5;219m");
}

static Highlight HIGHLIGHTS[] = {
    {
        .active = true,
        .trigger = {.type = TOK_STAR, .count = 1},
        .terminator = {.type = TOK_STAR, .count = 1},
        .text = HL_YELL,
        .start = HL_HIDE,
        .term = HL_HIDE,
    },
    {
        .active = true,
        .trigger = {.type = TOK_STAR, .count = 2},
        .terminator = {.type = TOK_STAR, .count = 2},
        .text = HL_RED_BOLD,
        .start = HL_HIDE,
        .term = HL_HIDE,
    },
    {
        .active = true,
        .trigger = {.type = TOK_QUOTE, .count = 1},
        .terminator = {.type = TOK_QUOTE, .count = 1},
        // TODO: if they are the same, it can be unified
        .text = HL_GREEN,
        .start = HL_GREEN,
        .term = HL_GREEN,
    },
    {
        .active = true,
        .trigger = {.type = TOK_BOPEN, .count = 2},
        .terminator = {.type = TOK_BCLOSE, .count = 2},
        .text = HL_BLUE,
        .start = HL_HIDE_STRONG,
        .term = HL_HIDE_STRONG,
    },
    {
        .active = true,
        .trigger = {.type = TOK_EQ, .count = 2},
        .terminator = {.type = TOK_EQ, .count = 2},
        .text = HL_BLACK_ON_YELL,
        .start = HL_YELL_ON_YELL,
        .term = HL_YELL_ON_YELL,
    },
    {
        .active = true,
        .trigger = {.type = TOK_STRIKE, .count = 1},
        .terminator = {.type = TOK_STRIKE, .count = 1},
        .text = HL_STRIKE,
        .start = HL_STRIKE,
        .term = HL_STRIKE,
    },
    /* code blocks */
    {
        .active = true,
        .trigger = {.type = TOK_TICK, .count = 1},
        .terminator = {.type = TOK_TICK, .count = 1},
        .text = HL_GRAY_ON_BLACK,
        .start = HL_GRAY_ON_BLACK_HIDE,
        .term = HL_GRAY_ON_BLACK_HIDE,
    },
    {
        .active = true,
        .trigger = {.type = TOK_TICK, .count = 3},
        .terminator = {.type = TOK_TICK, .count = 3},
        .text = HL_GRAY_ON_BLACK,
        .start = HL_GRAY_ON_BLACK_HIDE,
        .term = HL_GRAY_ON_BLACK_HIDE,
    },
    /* arrows - runs these before the carrots, else they have wrong prio */
    {
        .active = true,
        .trigger = {.type = TOK_DASH, .count = 1, .lookahead = {TOK_CARRET}},
        .terminator = {.type = TOK_CARRET, .count = 1},
        .text = HL_PURPLE,
        .start = HL_PURPLE,
        .term = HL_PURPLE,
    },
    {
        .active = true,
        .trigger = {.type = TOK_EQ, .count = 1, .lookahead = {TOK_CARRET}},
        .terminator = {.type = TOK_CARRET, .count = 1},
        .text = HL_YELL_BOLD,
        .start = HL_YELL_BOLD,
        .term = HL_YELL_BOLD,
    },
    /* comment & quote blocks */
    {
        .active = true,
        .trigger = {.type = TOK_CARRET, .count = 1},
        .terminator = {.type = TOK_NL, .count = 1},
        .text = HL_DARK_GRAY,
        .start = HL_RED,
        .term = HL_NONE,
    },
    {
        .active = true,
        .trigger = {.type = TOK_CARRET, .count = 2},
        .terminator = {.type = TOK_NL, .count = 1},
        .text = HL_YELL_ON_PINK,
        .start = HL_RED, // HL_RED_ON_PINK,
        .term = HL_NONE,
    },
    {
        .active = true,
        .trigger = {.type = TOK_HASH, .count = 1, .lookahead = {TOK_SPACE}},
        .terminator = {.type = TOK_NL, .count = 1},
        .text = HL_RED_ON_RED_UNDER,
        .start = HL_RED_ON_RED_UNDER,
        .term = HL_NONE,
    },
    {
        .active = true,
        .trigger = {.type = TOK_HASH, .count = 2},
        .terminator = {.type = TOK_NL, .count = 1},
        .text = HL_RED_UNDER,
        .start = HL_RED_UNDER,
        .term = HL_NONE,
    },
    {
        .active = true,
        .trigger = {.type = TOK_HASH, .count = 3},
        .terminator = {.type = TOK_NL, .count = 1},
        .text = HL_RED,
        .start = HL_HIDE_STRONG,
        .term = HL_NONE,
    },
    {
        .active = true,
        .trigger = {.type = TOK_HASH, .count = 4},
        .terminator = {.type = TOK_NL, .count = 1},
        .text = HL_LIGHTER_RED,
        .start = HL_HIDE_STRONG,
        .term = HL_NONE,
    },
    {
        .active = true,
        .trigger = {.type = TOK_HASH, .count = 5},
        .terminator = {.type = TOK_NL, .count = 1},
        .text = HL_LIGHTEST_RED,
        .start = HL_HIDE_STRONG,
        .term = HL_NONE,
    },
    /* TAGS */
    {
        .active = true,
        .nl_termination = true,
        .trigger = {.type = TOK_HASH, .count = 1},
        .terminator = {.type = TOK_SPACE, .count = 1},
        .text = HL_RED_ON_RED,
        .start = HL_RED_ON_RED,
        .term = HL_NONE,
    },
    /* Open todo */
    {
        .active = true,
        .trigger = {.type = TOK_BOPEN,
                    .count = 1,
                    .lookahead = {TOK_SPACE, TOK_BCLOSE, TOK_SPACE}},
        .terminator = {.type = TOK_BCLOSE, .count = 1},
        .text = HL_DARK_GREEN,
        .start = HL_DARK_GREEN,
        .term = HL_DARK_GREEN,
    },
    /* Closed todo */
    {
        .active = true,
        .trigger = {.type = TOK_BOPEN,
                    .count = 1,
                    .lookahead = {TOK_TEXT, TOK_BCLOSE, TOK_SPACE}},
        .terminator = {.type = TOK_BCLOSE, .count = 1},
        .text = HL_LGREEN_ON_DGREEN,
        .start = HL_LGREEN_ON_DGREEN,
        .term = HL_LGREEN_ON_DGREEN,
    },
    /* unordered list */
    {
        .active = true,
        .trigger = {.type = TOK_DASH, .count = 1, .lookahead = {TOK_SPACE}},
        .terminator = {.type = TOK_SPACE, .count = 1},
        .text = HL_LIGHT_ON_PINK,
        .start = HL_LIGHT_ON_PINK,
        .term = HL_LIGHT_ON_PINK,
    },

};
