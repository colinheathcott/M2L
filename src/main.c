#include "utils/list.h"
#include "utils/source.h"
#include "utils/ansi.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

int main(int argc, char **argv) {
    InitConsoleColors();

    const Source source = SourceNewFromData("func main()");
    const Span span = (Span) {.src = &source, 
        .offset = 0, .length = 11, .x = 1, .y = 1};
    const Substring substr = SpanSubstring(&span);
    
    // Sanity checks
    assert(!SubstringIsNull(&substr));
    assert(substr.length == 11);
    assert(strcmp(source.path, "<static_data>") == 0);

    for (int i = 0; i < substr.length; i++)
        putchar(substr.data[i]);
    putchar('\n');

    List intList = ListNew(sizeof(int), 2);
    assert(ListIsValid(&intList));
    int a = 1;
    int b = 2;
    int c = 3;
    assert(ListPush(&intList, &a) == LIST_RES_OK);
    assert(ListPush(&intList, &b) == LIST_RES_OK);
    assert(ListPush(&intList, &c) == LIST_RES_REALLOC);
    assert(*(int *)(ListGet(&intList, 0)) == a);

    printf("%sRed %s%sGreen %s%sBlue%s",
        ANSI_COLOR_RED,
        ANSI_RESET,
        ANSI_COLOR_GREEN,
        ANSI_RESET,
        ANSI_COLOR_BLUE,
        ANSI_RESET
    );

    return 0;
}