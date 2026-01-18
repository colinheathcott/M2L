#ifndef EXPR_H
#define EXPR_H

#include "ast.h"
#include "../common/list.h"
#include "../common/source.h"
#include <stdint.h>

typedef struct ExprCall {
    ExprId callee;
    List   args; // List<ExprId>
} ExprCall;

typedef struct ExprUnary {
    ExprId operand;
    const char *op;
} ExprUnary;

typedef struct ExprBinary {
    ExprId lhs;
    ExprId rhs;
    const char *op;
} ExprBinary;

typedef struct ExprBinary ExprLogical;
typedef struct ExprBinary ExprCompare;
typedef struct ExprBinary ExprEquality;
typedef struct ExprBinary ExprAssign;

#define AST_EXPR_LIST                                                          \
    X(EXPR_SYMBOL,   "EXPR_SYMBOL")                                            \
    X(EXPR_INT,      "EXPR_INT")                                               \
    X(EXPR_FLOAT,    "EXPR_FLOAT")                                             \
    X(EXPR_BOOL,     "EXPR_BOOL")                                              \
    X(EXPR_STR,      "EXPR_STR")                                               \
    X(EXPR_CALL,     "EXPR_CALL")                                              \
    X(EXPR_UNARY,    "EXPR_UNARY")                                             \
    X(EXPR_BINARY,   "EXPR_BINARY")                                            \
    X(EXPR_LOGICAL,  "EXPR_LOGICAL")                                           \
    X(EXPR_COMPARE,  "EXPR_COMPARE")                                           \
    X(EXPR_EQUALITY, "EXPR_EQUALITY")                                          \
    X(EXPR_ASSIGN,   "EXPR_ASSIGN")                 

typedef enum ExprKind {
    #define X(name, str) name,
    AST_EXPR_LIST
    #undef X
} ExprKind;

// Returns the expression kind name as a string.
const char *ExprKindStr(const ExprKind kind);

typedef union ExprData {
    Substring    exprSymbol;
    int64_t      exprInt;
    double       exprFloat;
    bool         exprNool;
    Substring    exprStr;
    ExprCall     exprCall;
    ExprUnary    exprUnary;
    ExprBinary   exprBinary;
    ExprLogical  exprLogical;
    ExprCompare  exprCompare;
    ExprEquality exprEquality;
    ExprAssign   exprAssign;
} ExprData;

typedef struct Expression {
    const Span span;
    const ExprKind kind;
    const ExprData data;
} Expression;

// Pushes the given expression to the AST (copies the data in the pointer)
// and returns the index of said expression in the list.
ExprId AstExprPush(Ast *ast, const Expression *expr);

#endif