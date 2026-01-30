#include "ast.h"
#include "expr.h"
#include <stdio.h>

#define INIT_EXPR_CAPACITY   512
#define INIT_STMT_CAPACITY   128
#define INIT_DECL_CAPACITY   64
#define INIT_ROOT_CAPACITY   64
#define INIT_ARGS_CAPACITY   32
#define INIT_PARAMS_CAPACITY 16

// -------------------------------------------------------------------------- //
// MARK: AST
// -------------------------------------------------------------------------- //

Ast AstNew() {
    // Allocate the 4 lists
    List exprList   = ListNew(sizeof(Expression), INIT_EXPR_CAPACITY);
    List stmtList   = ListNew(sizeof(Expression), INIT_STMT_CAPACITY);
    List declList   = ListNew(sizeof(Expression), INIT_DECL_CAPACITY);
    List rootList   = ListNew(sizeof(Expression), INIT_ROOT_CAPACITY);
    List argsList   = ListNew(sizeof(ExprId), INIT_ARGS_CAPACITY);
    List paramsList = ListNew(sizeof(ExprId), INIT_PARAMS_CAPACITY);

    printf("exprs valid: %d\n", ListIsValid(&exprList));
    printf("stmts valid: %d\n", ListIsValid(&stmtList));
    printf("decls valid: %d\n", ListIsValid(&declList));
    printf("root valid: %d\n", ListIsValid(&rootList));
    printf("args valid:  %d\n", ListIsValid(&argsList));
    printf("params valid: %d\n", ListIsValid(&paramsList));

    Ast ast = {
        .exprs  = exprList,
        .stmts  = stmtList,
        .decls  = declList,
        .root   = rootList,
        .args   = argsList,
        .params = paramsList,
    };

    // Seed each list with the sentinel node, this will take the place of
    // the "null index" for each AST node id.
    Expression sentinelExpr = (Expression) {0};
    // Statement sentinelStmt = (Statement) {0};
    // Declration sentinelDecl = (Declaration) {0};

    /* discard */ ListPush(&ast.exprs, &sentinelExpr);
    /* discard */ ListPush(&ast.stmts, &sentinelExpr);
    /* discard */ ListPush(&ast.decls, &sentinelExpr);
    /* discard */ ListPush(&ast.root, &sentinelExpr);
    // /* discard */ ListPush(&ast.stmts, &sentinelStmt);
    // /* discard */ ListPush(&ast.decls, &sentinelDecl);

    // Should this be a program node?
    // /* discard */ ListPush(&ast.root, &sentinelDecl);

    // Make sure these are all valid
    if (!AstIsValid(&ast)) {
        // Poison and return
        return (Ast) {0};
    }

    // Return the seeded AST
    return ast;
}

bool AstIsValid(const Ast *self) {
    if (!self) return false;
    
    bool listsValid = (
        ListIsValid(&self->exprs)
        && ListIsValid(&self->stmts)
        && ListIsValid(&self->decls)
        && ListIsValid(&self->root)
        && ListIsValid(&self->args)
        && ListIsValid(&self->params)
    );
    
    bool listsHaveSentinels = (
        self->exprs.count >= 1
        && self->stmts.count >= 1
        && self->decls.count >= 1
        && self->root.count >= 1
    );

    return (listsValid && listsHaveSentinels);
}