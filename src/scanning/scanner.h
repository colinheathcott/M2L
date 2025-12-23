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
    List tokenList;
} Scanner;

Scanner ScannerNew(const Source *src);
List ScannerScanSource(Scanner *self);
void ScannerDestroy(Scanner *self);

#endif