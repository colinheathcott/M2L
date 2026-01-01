#include "scanner.h"
#include "../utils/list.h"
#include "../utils/diag.h"
#include "token.h"
#include <ctype.h>
#include <stdio.h>

#ifdef LOG_LEXER
#define LOG(fmt, ...) fprintf(stderr, fmt __VA_OPT__(,) __VA_ARGS__)
#else
#define LOG(fmt, ...) ((void)0)
#endif

// -------------------------------------------------------------------------- //
// MARK: Helpers
// -------------------------------------------------------------------------- //

static bool isSymbolStart(char ch) {
    return isalpha(ch) || ch == '_';
}

static bool isSymbolFollow(char ch) {
    return isalnum(ch) || ch == '_';
}

static bool isDigitStart(char ch) { 
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
    LOG("CALL INTO NEXT\n");
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
    LOG("CALL INTO SKIP WS\n");
    char ch = current(self);
    while (ch == ' ' || ch == '\t' || ch == '\r') {
        next(self);
        ch = current(self);
    }
}

// -------------------------------------------------------------------------- //
// MARK: Scanner Methods
// * All scanner methods assume that `self` pointer is valid since they can
// * only be called from `ScannerScan()` which should check for validity
// * prior to calling any other methods.
// -------------------------------------------------------------------------- //

// assumes current -> '\n'
static void scanEol(Scanner *self) {  
    self->y++;
    self->x = 0;
}

static void scanSymbol(Scanner *self) {
    size_t offset = self->offset;
    size_t x = self->x;
    size_t y = self->y;
    
    while (isSymbolFollow(peek(self))) {
        next(self);
    }

    // Here, the next item is NOT part of the symbol
    // First get a span and check the substring
    size_t length          = self->offset - offset + 1;
    const Span span        = (Span) {self->src, offset, length, x, y};

    // Check if the substring is a keyword
    const TokenKind kind = cmpKeywords(self, &span);

    // Create the token and push
    const Token token = (Token) { kind, span };
    TLPush(self->tokenList, &token);

    // Advance to point to the next token
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
    LOG("BEFORE: '%c'\n", current(self));
    skipWhitespace(self);

    size_t offset = self->offset;
    size_t x = self->x;
    size_t y = self->y;
    size_t length = 1;
    
    unsigned char ch = current(self);
    LOG("START: '%c'\n", ch);

    // Look for symbols
    if (isSymbolStart(ch)) {
        scanSymbol(self);
        return;
    }

    TokenKind kind = 0;
    switch (ch) {
    case '\0': {
        kind = TK_EOF;
        self->scanning = false;
        break;
    }
    case '\n': {
        scanEol(self);
        kind = TK_EOL;
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

    case '=': {
        if (expect(self, '=')) {
            length++;
            kind = TK_EQ_EQ;
        } else {
            kind = TK_EQ;
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
        self->success = false;
        DEPush(self->diagEngine, &diag);
        next(self);
        return;
    }
    }

    const Span span = (Span) {self->src, offset, length, x, y};
    const Token token = (Token) { kind, span };

    // Consume the number of characters of the token
    LOG("LENGTH: %zu\n", length);
    for (size_t i = 0; i < length; i++) {
        next(self);
    }   

    // Emit the token and return
    TLPush(self->tokenList, &token);

    ch = current(self);
    LOG("END: '%c'\n", ch);
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
        .src        = src,
        .x          = 1,
        .y          = 1,
        .offset     = 0,
        .tokenList  = tokenList,
        .diagEngine = diagEngine,
        .scanning   = false,
        .success    = true,
    };

    if (ScannerIsValid(&s)) return s;
    else return (Scanner) {0};
}

void ScannerScan(Scanner *self, bool *success) {
    // @(expect) assume `success` is not nul.
    if (!success) {
        printf("out pointer is NULL");
        return;
    }
    
    if (!ScannerIsValid(self)) {
        *success = false;
        return;
    }

    self->scanning = true;
    while (self->scanning) {
        scanToken(self);
    }

    *success = self->success;
    *self = (Scanner) {0}; // poison this scanner
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