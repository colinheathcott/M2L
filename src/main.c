#include "utils/list.h"
#include "utils/source.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

int main(int argc, char **argv) {
    const Source source = Source_New_From_Data("func main()");
    const Span span = (Span) {.offset = 0, .length = 11, .x = 1, .y = 1};
    const Substring substr = Source_Substring(&source, &span);
    
    // Sanity checks
    assert(!Substring_Is_Null(&substr));
    assert(substr.length == 11);
    assert(strcmp(source.path, "<static_data>") == 0);

    for (int i = 0; i < substr.length; i++)
        putchar(substr.data[i]);
    putchar('\n');

    List int_list = List_New(sizeof(int), 2);
    assert(List_Is_Valid(&int_list));
    int a = 1;
    int b = 2;
    int c = 3;
    assert(List_Push(&int_list, &a) == LIST_RES_OK);
    assert(List_Push(&int_list, &b) == LIST_RES_OK);
    assert(List_Push(&int_list, &c) == LIST_RES_REALLOC);
    assert(*(int *)(List_Get(&int_list, 0)) == a);

    return 0;
}