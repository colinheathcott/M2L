#ifndef SCANNER_H
#define SCANNER_H

#include "../common/source.h"
#include "../common/diag.h"
#include "token.h"
#include <stdbool.h>

#define LOG_LEXER

typedef struct Scanner {
    const Source *src;
    size_t x;
    size_t y;
    size_t offset;
    TokenList *tokenList;
    DiagEngine *diagEngine;
    bool scanning;
    bool success;
} Scanner;

Scanner ScannerNew(const Source *src, DiagEngine *diagEngine,
    TokenList *tokenList);

// Always initialize `success` to `false` at the caller level to ensure 
// the scanner fails if the `success` pointer is null.
void Scan(Scanner *self, bool *success);

// Determines whether or not the scanner is valid, i.e. does it have a valid
// source file, diag list, token list, etc.
bool ScannerIsValid(const Scanner *self);

#endif