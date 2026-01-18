#include "parser.h"
#include "../common/list.h"

// -------------------------------------------------------------------------- //
// MARK: Parser Helpers
// -------------------------------------------------------------------------- //

// Returns the number of tokens in this parser's token list.
size_t count(const Parser *self) {
    return self->tokenList->tokens.count;
}

// The token pointer returned from this function is guaraunteed to not be `NULL`
Token *get(const Parser *self, size_t k) {
    if (self->cursor + k >= count(self)) {
        return ((Token *)ListBack(&self->tokenList->tokens));
    }
    return (Token *)ListGet(&self->tokenList->tokens, self->cursor + k);
}

// Advances the parser `k` tokens ahead. Will automatically prevent `cursor` 
// from being greater than the token list count.
void next(Parser *self, size_t k) {
    // Prevent the cursor from overflowing the length of the token list
    if (self->cursor + k >= count(self)) {
        self->cursor = count(self);
        return;
    }
    self->cursor += k;
}

// -------------------------------------------------------------------------- //
// MARK: Parser API
// -------------------------------------------------------------------------- //

Parser ParserNew(
    const Source *src,
    Ast *ast,
    DiagEngine *diagEngine,
    const TokenList *tokenList
) {
    if (!src
        || !ast
        || !diagEngine
        || !tokenList
        || tokenList->tokens.count == 0
    ) {
        return (Parser) {0};
    }

    return (Parser) {
        .src = src,
        .diagEngine = diagEngine,
        .tokenList = tokenList,
        .ast = ast,
        .cursor = 0,
    };
}

bool ParserIsValid(const Parser *self) {
    return (self
        && self->ast
        && self->diagEngine
        && self->tokenList
        && ListIsValid(&self->tokenList->tokens)
        && ListIsValid(&self->diagEngine->diagnostics)
        && AstIsValid(self->ast)
    );
}