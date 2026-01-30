#ifndef AST_H
#define AST_H

#include "../common/list.h"
#include <stdbool.h>
#include <stdint.h>

#define NULL_AST_ID 0

// -------------------------------------------------------------------------- //
// MARK: Types
// -------------------------------------------------------------------------- //

// Used to index into a list of expressions.
typedef size_t ExprId;

// Used to index into a list of statements.
typedef size_t StmtId;

// Used to index into a list of declaration.
typedef size_t DeclId;

// -------------------------------------------------------------------------- //
// MARK: AST
// -------------------------------------------------------------------------- //

// Stores all the data/information for an abstract syntax tree. This is source
// agnostics and only holds lists of nodes.
typedef struct Ast {
    List exprs;  // `List<Expression>`
    List stmts;  // `List<Statement>`
    List decls;  // `List<Declaration>`
    List root;   // `List<Declaration>` (ordered)
    // --------- Side Tables ---------
    List args;   // `List<ExprId>`
    List params; // `List<ExprId>`
} Ast;

// Creates a new blank AST. Please verify allocation with `AstIsValid()`.
Ast AstNew();

// Checks that every list in the AST is valid and that each one has a
// successfully allocated sentinel on the front.
bool AstIsValid(const Ast *self);

#endif