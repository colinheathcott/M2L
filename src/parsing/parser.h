#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "../common/source.h"
#include "../common/diag.h"
#include "../scanning/token.h"
#include <stdbool.h>

#define LOG_PARSER

// -------------------------------------------------------------------------- //
// MARK: Parser
// -------------------------------------------------------------------------- //

// Used to traverse the token list and create an AST for the `src` fie.
typedef struct Parser {
    // The translaton unit.
    const Source *src;

    // The input stream of tokens.
    const TokenList *tokenList;

    // A pointer to the diagnostic collection.
    DiagEngine   *diagEngine;

    // A pointer to the `Ast` holding all the allocated nodes and ata.
    Ast *ast;

    // Points to where the parser is in the token stream.
    size_t cursor;
} Parser;

// Creates a new parser for the translation unit and token list.
Parser ParserNew(const Source *src, Ast *ast, DiagEngine *diagEngine,
    const TokenList *tokenList);

// Parses the entire token stream and attempts to construct an AST. Will
// modify the `success` pointer if there were errors during this process.
void Parse(Parser *self, bool *success);

// Returns whether or not the data being used in the parser is valid or not.
bool ParserIsValid(const Parser *self);

// -------------------------------------------------------------------------- //
// MARK: Testing Interface
// -------------------------------------------------------------------------- //

#ifdef M2l_TEST_IMPL
ExprId expression(Parser *self);
#endif

#endif
