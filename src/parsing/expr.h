#ifndef EXPR_H
#define EXPR_H

#include "ast.h"
#include "../common/list.h"
#include "../common/source.h"
#include <stdint.h>

// -------------------------------------------------------------------------- //
// MARK: Components
// -------------------------------------------------------------------------- //

typedef struct ExprCall {
    ExprId callee;
    
    // Index into the args vector
    size_t argid;
    
    // Number of args
    size_t argc;
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

// -------------------------------------------------------------------------- //
// MARK: Variants
// -------------------------------------------------------------------------- //

#define AST_EXPR_LIST                                                          \
    X(EXPR_SYMBOL,   "EXPR_SYMBOL")                                            \
    X(EXPR_INT,      "EXPR_INT")                                               \
    X(EXPR_FLOAT,    "EXPR_FLOAT")                                             \
    X(EXPR_BOOL,     "EXPR_BOOL")                                              \
    X(EXPR_STR,      "EXPR_STR")                                               \
    X(EXPR_CALL,     "EXPR_CALL")                                              \
    X(EXPR_POSTFIX,  "EXPR_POSTFIX")                                           \
    X(EXPR_PREFIX,   "EXPR_PREFIX")                                            \
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

// -------------------------------------------------------------------------- //
// MARK: Expression
// -------------------------------------------------------------------------- //

// Holds some data for an expression. Which variant to use is determined by the
// `kind` of the expression.
typedef union ExprData {
    Substring    exprSymbol;
    int64_t      exprInt;
    double       exprFloat;
    bool         exprBool;
    Substring    exprString;
    ExprCall     exprCall;
    ExprUnary    exprUnary;
    ExprBinary   exprBinary;
} ExprData;

// Holds the data for one AST expression.
typedef struct Expression {
    // The source code span of this expression.
    const Span span;

    // The variant of this expression.
    const ExprKind kind;

    // The underlying data for the expression.
    const ExprData data;
} Expression;

// Pushes the given expression to the AST (copies the data in the pointer)
// and returns the index of said expression in the list.
ExprId AstExprPush(Ast *ast, const Expression *expr);

// Returns te expression given by the provided index. Will fatally error if
// the index is invalid or out of bounds.
Expression *AstExprGet(const Ast *self, ExprId id);

#endif
