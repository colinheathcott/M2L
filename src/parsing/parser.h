#ifndef PARSER_H
#define PARSER_H
#include "../common/source.h"
#include "../common/diag.h"
#include "../scanning/token.h"
#include "ast.h"
#include <stdbool.h>

#define LOG_PARSER

typedef struct Parser {
    const Source *src;
    const TokenList *tokenList;
    DiagEngine   *diagEngine;
    Ast *ast;
    size_t cursor;
} Parser;

Parser ParserNew(const Source *src, Ast *ast, DiagEngine *diagEngine,
    const TokenList *tokenList);

void Parse(Parser *self, bool *success);

bool ParserIsValid(const Parser *self);

#endif