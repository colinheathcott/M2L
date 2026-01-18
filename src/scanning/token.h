#ifndef TOKEN_H
#define TOKEN_H

#include "../common/source.h"
#include "../common/list.h"

#define INIT_TOKEN_LIST_CAP 512

// -------------------------------------------------------------------------- //
// MARK: Token
// -------------------------------------------------------------------------- //

#define TOKEN_LIST                                                             \
    X(TK_LPAR, "LPAREN")                                                       \
    X(TK_RPAR, "RPAREN")                                                       \
    X(TK_LCURL, "LCURL")                                                       \
    X(TK_RCURL, "RCURL")                                                       \
    X(TK_LBRAC, "LBRAC")                                                       \
    X(RK_RBRAC, "RBRAC")                                                       \
                                                                               \
    X(TK_PLUS, "PLUS")                                                         \
    X(TK_MIN, "MIN")                                                           \
    X(TK_STAR, "STAR")                                                         \
    X(TK_SLASH, "SLASH")                                                       \
    X(TK_PLUS_PLUS, "PLUS_PLUS")                                               \
    X(TK_MIN_MIN, "MIN_MIN")                                                   \
    X(TK_STAR_STAR, "STAR_STAR")                                               \
    X(TK_PLUS_EQ, "PLUS_EQ")                                                   \
    X(TK_MINUS_EQ, "MINUS_EQ")                                                 \
    X(TK_STAR_EQ, "STAR_EQ")                                                   \
    X(TK_STAR_STAR_EQ, "STAR_STAR_EQ")                                         \
    X(TK_SLASH_SLASH, "SLASH_SLASH")                                           \
    X(TK_SLASH_EQ, "SLASH_EQ")                                                 \
    X(TK_SLASH_SLASH_EQ, "SLASH_SLASH_EQ")                                     \
    X(TK_PERCENT, "PERCENT")                                                   \
    X(TK_PERCENT_EQ, "PERCENT_EQ")                                             \
    X(TK_BANG, "BANG")                                                         \
    X(TK_BANG_EQ, "BANG_EQ")                                                   \
    X(TK_EQ, "EQ")                                                             \
    X(TK_EQ_EQ, "EQ_EQ")                                                       \
    X(TK_LT, "LT")                                                             \
    X(TK_LT_EQ, "LT_EQ")                                                       \
    X(TK_GT, "GT")                                                             \
    X(TK_GT_EQ, "GT_EQ")                                                       \
    X(TK_PIPE, "PIPE")                                                         \
    X(TK_PIPE_PIPE, "PIPE_PIPE")                                               \
    X(TK_AND, "AND")                                                           \
    X(TK_AND_AND, "AND_AND")                                                   \
    X(TK_COLON, "COLON")                                                       \
    X(TK_SEMICOLON, "SEMICOLON")                                               \
    X(TK_DOT, "DOT")                                                           \
    X(TK_QMARK, "QMARK")                                                       \
    X(TK_COMMA, "COMMA")                                                       \
    X(TK_BACKTICK, "BACKTICK")                                                 \
    X(TK_ARROW, "ARROW")                                                       \
                                                                               \
    X(TK_SYMBOL, "SYMBOL")                                                     \
    X(TK_INT, "INT")                                                           \
    X(TK_STR, "STR")                                                           \
    X(TK_FLOAT, "FLOAT")                                                       \
                                                                               \
    X(TK_FUN, "FUN")                                                           \
    X(TK_LET, "LET")                                                           \
    X(TK_MUT, "MUT")                                                           \
    X(TK_ENUM, "ENUM")                                                         \
    X(TK_TYPE, "TYPE")                                                         \
    X(TK_IF, "IF")                                                             \
    X(TK_ELSE, "ELSE")                                                         \
    X(TK_FOR, "FOR")                                                           \
    X(TK_IN, "IN")                                                             \
    X(TK_WHILE, "WHILE")                                                       \
    X(TK_EOF, "EOF")

typedef enum TokenKind {
    #define X(name, str) name,
    TOKEN_LIST
    #undef X
} TokenKind;

typedef struct Token {
    const TokenKind kind;
    const Span      span;
} Token;

void TokenPrint(FILE *ioStream, const Token *self);

// -------------------------------------------------------------------------- //
// MARK: TokenList
// -------------------------------------------------------------------------- //

typedef struct TokenList {
    List tokens;
} TokenList;

TokenList TLNew();
void TLPush(TokenList *self, const Token *token);
void TLPrint(FILE *ioStream, const TokenList *self);


#endif