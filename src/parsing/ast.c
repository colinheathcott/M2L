#include "ast.h"
#include "expr.h"

#define INIT_EXPR_CAPACITY 512
#define INIT_STMT_CAPACITY 128
#define INIT_DECL_CAPACITY 64
#define INIT_ROOT_CAPACITY 64

Ast AstNew() {
    // Allocate the 4 lists
    List exprList = ListNew(sizeof(Expression), INIT_EXPR_CAPACITY);
    List stmtList = ListNew(sizeof(Expression), INIT_STMT_CAPACITY);
    List declList = ListNew(sizeof(Expression), INIT_DECL_CAPACITY);
    List rootList = ListNew(sizeof(Expression), INIT_ROOT_CAPACITY);

    return (Ast) {
        .exprs = exprList,
        .stmts = stmtList,
        .decls = declList,
        .root  = rootList,
    };
}

bool AstIsValid(const Ast *self) {
    return (self
        && ListIsValid(&self->exprs)
        && ListIsValid(&self->stmts)
        && ListIsValid(&self->decls)
        && ListIsValid(&self->root));
}

ExprId AstExprPush(Ast *self, const Expression *expr) {
    if (!self || !ListIsValid(&self->exprs))
        return NULL_AST_ID;

    // @(expect) assume ListPush works
    /* discard */ ListPush(&self->exprs, expr);
    return self->exprs.count - 1;
}