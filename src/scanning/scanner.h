#ifndef SCANNER_H
#define SCANNER_H

#include "../utils/source.h"
#include "../utils/diag.h"
#include "token.h"

typedef struct Scanner {
    const Source *src;
    size_t x;
    size_t y;
    size_t offset;
    TokenList *tokenList;
    DiagEngine *diagEngine;
    bool scanning;
} Scanner;

Scanner ScannerNew(const Source *src, DiagEngine *diagEngine,
    TokenList *tokenList);

void ScannerScanSource(Scanner *self);
void ScannerDestroy(Scanner *self);
bool ScannerIsValid(const Scanner *self);

#endif