#ifndef TOKEN_H
#define TOKEN_H

#include "../utils/source.h"

typedef enum TokenKind {
    TK_EOF = 0,
    TK_EOL,

    /* Grouping */
    TK_LPAR, TK_RPAR, TK_LBRAC, TK_RBRAC, TK_LCURL, TK_RCURL,
    
    /* Arithmetic */
    TK_PLUS, TK_PLUS_PLUS, TK_PLUS_EQ,
    TK_MIN, TK_MIN_MIN, TK_MIN_EQ,
    TK_STAR, TK_STAR_STAR, TK_STAR_EQ, TK_STAR_STAR_EQ,
    TK_SLASH, TK_SLASH_SLASH, TK_SLASH_EQ, TK_SLASH_SLASH_EQ,
    TK_MOD, TK_MOD_EQ,

    /* Comparison */
    TK_LT, TK_GT, TK_LT_EQ, TK_GT_EQ,
    TK_BANG, TK_BANG_EQ, TK_EQ, TK_EQ_EQ,
    TK_AND, TK_AND_AND, TK_BAR, TK_BAR_BAR,

    /* Misc Operators */
    TK_DOT, TK_COMMA, TK_COLON, TK_SEMICOLON, TK_ARROW,

    /* Literals */
    TK_STRING, TK_INT, TK_FLOAT, TK_SYMBOL,

    /* Keywords */
    TK_LET, TK_MUT, TK_FUNC, TK_TYPE, TK_IF, TK_ELSE,
    TK_FOR, TK_IN, TK_WHILE
} TokenKind;

typedef struct Token {
    const TokenKind kind;
    const Span      span;
} Token;

#endif