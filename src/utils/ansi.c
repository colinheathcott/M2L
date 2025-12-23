#include "ansi.h"

bool USE_ANSI_FMT_SEQUENCES = false;

#ifdef _WIN32
#include <windows.h>
#include <stdio.h>
void InitConsoleColors(void) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
        return;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
        return;

    if (dwMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) {
        USE_ANSI_FMT_SEQUENCES = true;
        return;
    }

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (SetConsoleMode(hOut, dwMode)) {
        USE_ANSI_FMT_SEQUENCES = true;
    }
}
#else
#include <unistd.h>
void InitConsoleColors() {
    if (isatty(STDOUT_FILENO))
        USE_ANSI_FMT_SEQUENCES = true;
}
#endif
