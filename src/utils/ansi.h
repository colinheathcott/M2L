#ifndef ANSI_H
#define ANSI_H
#include <stdbool.h>

extern bool USE_ANSI_FMT_SEQUENCES;

#define ANSI_ESC           (USE_ANSI_FMT_SEQUENCES ? "\033["  : "")
#define ANSI_RESET         (USE_ANSI_FMT_SEQUENCES ? "\033[m" : "")
#define ANSI_COLOR_RED     (USE_ANSI_FMT_SEQUENCES ? "\033[31m" : "")
#define ANSI_COLOR_GREEN   (USE_ANSI_FMT_SEQUENCES ? "\033[32m" : "")
#define ANSI_COLOR_YELLOW  (USE_ANSI_FMT_SEQUENCES ? "\033[33m" : "")
#define ANSI_COLOR_BLUE    (USE_ANSI_FMT_SEQUENCES ? "\033[34m" : "")
#define ANSI_COLOR_MAGENTA (USE_ANSI_FMT_SEQUENCES ? "\033[35m" : "")
#define ANSI_COLOR_CYAN    (USE_ANSI_FMT_SEQUENCES ? "\033[36m" : "")
#define ANSI_STYLE_BOLD    (USE_ANSI_FMT_SEQUENCES ? "\033[1m"  : "")
#define ANSI_STYLE_ITALIC  (USE_ANSI_FMT_SEQUENCES ? "\033[3m"  : "")
#define ANSI_BG_RED        (USE_ANSI_FMT_SEQUENCES ? "\033[37;41m"  : "")
#define ANSI_BG_YELLOW     (USE_ANSI_FMT_SEQUENCES ? "\033[37;43m"  : "")
#define ANSI_BG_BLUE       (USE_ANSI_FMT_SEQUENCES ? "\033[37;44m"  : "")
#define COLORIZE(c)        printf("%s", (c))

void InitConsoleColors();

#endif