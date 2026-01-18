#include "expr.h"
#include <stdio.h>

const char *ExprKindStr(const ExprKind kind) {
    #define X(name, str) case name: return str;
    switch (kind) {
        AST_EXPR_LIST
        default: return "<invalid ExprKind>";
    }
    #undef X
}

ExprId AstExprPush(Ast *ast, const Expression *expr) {
    ListResult res = ListPush(&ast->exprs, expr);
    if (res != LIST_RES_OK || res != LIST_RES_REALLOC) {
        fprintf(stderr, "<error allocating an AST expression>\n");
        return 0;
    }
    return ast->exprs.count - 1;
}
