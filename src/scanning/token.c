#include "token.h"
#include <stdio.h>

const char *stringifyTokenKind(const TokenKind tk) {
    switch (tk) {
    #define X(name, str) case name: return str;
    TOKEN_LIST
    #undef X
    default: return "<invalid TokenKind>";
    }
}

void TokenPrint(FILE *ioStream, const Token *self) {
    // format: FILE:Y:X KIND 'LEXEME'
    const char *kind = stringifyTokenKind(self->kind);
    fprintf(ioStream,
        "[%s:%zu:%zu] %s '",
        self->span.src->path,
        self->span.y,
        self->span.x,
        kind
    );

    // Print <EOF> if it's EOF (invalid substring will error)
    if (self->kind == TK_EOF) {
        fprintf(ioStream, "\\0");

    // Otherwise get the substring
    } else {
        const Substring substr = SpanSubstring(&self->span);
        if (SubstringIsNull(&substr)) {
            fprintf(stderr, "<INVALID TOKEN SUBSTRING>\n");
            return;
        }
        SubstringPrint(ioStream, &substr);
    }
    
    fprintf(ioStream,"'\n");
}

TokenList TLNew() {
    List tokens = ListNew(sizeof(Token), INIT_TOKEN_LIST_CAP);
    
    if (!ListIsValid(&tokens))
        return (TokenList) { NULL_LIST };
    
    return (TokenList) { .tokens = tokens };
}

void TLPush(TokenList *self, const Token *token) {
    if (!self || !ListIsValid(&self->tokens))
        return;

    // @(expect) assume ListPush works
    /* discard */ ListPush(&self->tokens, token);
}

void TLPrint(FILE *ioStream, const TokenList *self) {
    if (!self || !ioStream || !ListIsValid(&self->tokens)) {
        fprintf(stderr, "<invalid token list pointer or IO stream pointer>\n");
        return;
    }

    for (size_t i = 0; i < self->tokens.count; i++) {
        Token *token = (Token *)ListGet(&self->tokens, i);
        if (!token) {
            fprintf(ioStream, "<invalid token pointer in list>\n");
            continue;
        }
        TokenPrint(ioStream, token);
    }
}