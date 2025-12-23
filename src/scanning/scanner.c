#include "scanner.h"
#include "token.h"
#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define SPAN(n) (Span) {.offset = offset, .length = (n), .x = x, .y = y}

static bool isSymbolStart(char ch) {
    return isalpha(ch) || ch == '_';
}

static bool isSymbolFollow(char ch) {
    return isalnum(ch) || ch == '_';
}

static bool isDigitStart(char ch){ 
    return isdigit(ch) || ch == '_';
}

static bool isDigitFollow(char ch) {
    return isdigit(ch) || ch == '_' || ch == '.';
}

static TokenKind cmpKeywords(const Scanner *self, const Span *span) {
    Substring str = SpanSubstring(span);
    // Purposefully skip the null check, the `SubstringCmpString()` will do it.
    if (SubstringCmpString(&str, "let"))   return TK_LET;
    if (SubstringCmpString(&str, "mut"))   return TK_MUT;
    if (SubstringCmpString(&str, "func"))  return TK_FUNC;
    if (SubstringCmpString(&str, "type"))  return TK_TYPE;
    if (SubstringCmpString(&str, "if"))    return TK_IF;
    if (SubstringCmpString(&str, "else"))  return TK_ELSE;
    if (SubstringCmpString(&str, "for"))   return TK_FOR;
    if (SubstringCmpString(&str, "in"))    return TK_IN;
    if (SubstringCmpString(&str, "while")) return TK_WHILE;
    return TK_SYMBOL;
}

static char current(const Scanner *self) {
    if (self->offset >= self->src->length)
        return '\0';
    return self->src->data[self->offset];
}

static char peek(const Scanner *self) {
    if (self->offset + 1 >= self->src->length)
        return '\0';
    return self->src->data[self->offset + 1];
}

static void next(Scanner *self) {
    if (self->offset >= self->src->length)
        return;
    self->offset++;
    self->x++;
}

static bool expect(Scanner *self, char ch) {
    if (peek(self) != ch)
        return false;
    next(self);
    return true;
}

static void skipWhitespace(Scanner *self) {
    char ch = current(self);
    while (ch == ' ' || ch == '\t' || ch == '\r') {
        next(self);
        ch = current(self);
    }
}

static void scanToken(Scanner *self) {
    skipWhitespace(self);
    
    size_t offset = self->offset;
    size_t length = 1;
    size_t x = self->x;
    size_t y = self->y;
    int kind = -1;

    switch (current(self)) {
    case '\0': {
        kind = TK_EOF;
        break;
    }
    default:
        kind = -1;
    }

    // Check for error (unknown character)
    if (kind < 0) {
        printf("Error encountered: %i", kind);
        return;
    }

    // Create and push the token by casting kind.
    Token tok = (Token) {
        .kind = (TokenKind)(kind),
        .span = (Span) { self->src, offset, length, x, y }
    };
    ListPush(&self->tokenList, &tok); /* discard */
}

Scanner ScannerNew(const Source *src) {
    // Make a new arena for the tokens
    List tokenList = ListNew(sizeof(Token), INIT_TOKEN_LIST_CAP);
    if (!ListIsValid(&tokenList))
        return (Scanner) {0};
    
    // Return valid scanner
    return (Scanner) {
        .src = src,
        .x = 1,
        .y = 1,
        .offset = 0,
        .tokenList = tokenList
    };
}

List ScannerScanSource(Scanner *self) {
    return self->tokenList;
}

void ScannerDestroy(Scanner *self) {
    if (!self) return;
    ListFree(&self->tokenList);
    *self = (Scanner) {0}; // poison
}