#ifndef PRINTER_H
#define PRINTER_H

#include "ast.h"
#include "../common/source.h"
#include <stdbool.h>

// -------------------------------------------------------------------------- //
// MARK: Printer
// -------------------------------------------------------------------------- //

// Used to print an AST to `stdout`.
typedef struct AstPrinter {
    // The source file (for substring fetching)
    const Source *src;

    // The AST to print.
    const Ast *ast;

    // The current indent level of the printer.
    size_t indent;
} AstPrinter;

// Create a new AST printer. Check validity with `AstPrinterIsValid()`.
AstPrinter AstPrinterNew(const Source *src, const Ast *ast);

// Returns whether or not the data inside the printer is valid.
bool AstPrinterIsValid(const AstPrinter *self);

// Print an expression.
void AstPrintExpr(AstPrinter *self, ExprId id);

#endif