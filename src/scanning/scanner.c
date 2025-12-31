#include "scanner.h"
#include "../utils/list.h"
#include "../utils/diag.h"
#include "token.h"
#include <ctype.h>
#include <stdio.h>

// -------------------------------------------------------------------------- //
// MARK: Helpers
// -------------------------------------------------------------------------- //

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

// -------------------------------------------------------------------------- //
// MARK: Scanner Helpers
// -------------------------------------------------------------------------- //

static unsigned char current(const Scanner *self) {
    if (self->offset >= self->src->length)
        return '\0';
    return self->src->data[self->offset];
}

static unsigned char peek(const Scanner *self) {
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

static bool expect(Scanner *self, unsigned char ch) {
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

// -------------------------------------------------------------------------- //
// MARK: Scanner Methods
// -------------------------------------------------------------------------- //

// assumes current -> '\n'
static void scanEol(Scanner *self) {  
    self->y++;
    self->x = 0;
    next(self);
}

/* Scans a single token if there is one and emits it. If no valid token is
 * found, then a diagnostic is emitted.
 *
 * Begins by matching the current character and ends on the first character of
 * the NEXT token. No need to advance from the caller, even if no token is
 * emitted.
 */
static void scanToken(Scanner *self) {
    skipWhitespace(self);

    size_t offset = self->offset;
    size_t x = self->x;
    size_t y = self->y;
    size_t length = 1;
    
    unsigned char ch = current(self);
    printf("scan loop: %c\n", ch);

    TokenKind kind = 0;
    switch (ch) {
    case '\0': {
        kind = TK_EOF;
        self->scanning = false;
        break;
    }

    case '(': { kind = TK_LPAR; break; }
    case ')': { kind = TK_RPAR; break; }
    
    case '+': {
        if (expect(self, '+')) {
            length++;
            kind = TK_PLUS_PLUS;
        } else if (expect(self, '=')) {
            length++;
            kind = TK_PLUS_EQ;
        } else {
            kind = TK_PLUS;
        }
        break;
    }

    case '.': { kind = TK_DOT;       break; }
    case ',': { kind = TK_COMMA;     break; }
    case ':': { kind = TK_COLON;     break; }
    case ';': { kind = TK_SEMICOLON; break; }

    default: {
        const DiagReport report = (DiagReport) {
            .span = (Span) {self->src, offset, 1, x, y},
            .message = ""
        };
        const Diagnostic diag = DiagNew(
            ERR_INVALID_CHAR,
            "this character is not recogonized",
            report
        );

        // Push the diagnostic and then early return
        DEPush(self->diagEngine, &diag);
        next(self);
        return;
    }
    }

    const Span span = (Span) {self->src, offset, length, x, y};
    const Token token = (Token) { kind, span };

    // Consume the number of characters of the token
    for (size_t i = length ;; i--) {
        if (i == 0) { break; }
        next(self);
    }

    // Emit the token and return
    TLPush(self->tokenList, &token);

    ch = current(self);
    printf("after loop: %c\n", ch);
    return;
}

// -------------------------------------------------------------------------- //
// MARK: Scanner API
// -------------------------------------------------------------------------- //

Scanner ScannerNew(
    const Source *src,
    DiagEngine *diagEngine,
    TokenList *tokenList
) {
    Scanner s = (Scanner) {
        .src = src,
        .x = 1,
        .y = 1,
        .offset = 0,
        .tokenList = tokenList,
        .diagEngine = diagEngine,
        .scanning = false
    };

    if (ScannerIsValid(&s)) return s;
    else return (Scanner) {0};
}

void ScannerScanSource(Scanner *self) {
    if (!ScannerIsValid(self)) return;

    self->scanning = true;
    while (self->scanning) {
        scanToken(self);
    }
}

void ScannerDestroy(Scanner *self) {
    if (!self) return;
    *self = (Scanner) {0}; // poison
}

bool ScannerIsValid(const Scanner *self) {
    return (!self
        || !self->src
        || !self->tokenList
        || !self->diagEngine
        || !ListIsValid(&self->diagEngine->diagList)
        || !ListIsValid(&self->tokenList->tokens)
    ) == false;
}