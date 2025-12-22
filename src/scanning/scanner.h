#ifndef SCANNER_H
#define SCANNER_H

#define INIT_TOKEN_LIST_CAP 512

#include "../utils/source.h"
#include "../utils/list.h"

typedef struct Scanner {
    const Source *src;
    size_t x;
    size_t y;
    size_t offset;
    List token_list;
} Scanner;

Scanner Scanner_New(const Source *src);
void Scanner_Destroy(Scanner *self);

#endif