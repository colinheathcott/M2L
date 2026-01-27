#include "common/list.h"
#include "common/source.h"
#include "common/ansi.h"
#include "common/diag.h"
#include "parsing/ast.h"
#include "parsing/parser.h"
#include "parsing/printer.h"
#include "scanning/token.h"
#include "scanning/scanner.h"
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

// -------------------------------------------------------------------------- //
// MARK: Main
// -------------------------------------------------------------------------- //

int main(int argc, char **argv) {
    InitConsoleColors();

    const Source source = SourceNewFromData("x && x > 5");
    DiagEngine de = DENew();
    TokenList  tl = TLNew();
    
    if (!ListIsValid(&tl.tokens) || !ListIsValid(&de.diagnostics)) return 1;
    
    // Make a new scanner
    Scanner scanner = ScannerNew(&source, &de, &tl);
    if (!ScannerIsValid(&scanner)) return 1;

    // Scan tokens
    bool scanSuccess = false;
    Scan(&scanner, &scanSuccess);
    if (!scanSuccess) {
        TLPrint(stderr, &tl);
        DEPrint(stderr, &de);
        return 1;
    }

    printf("%zu\n", tl.tokens.count);
    assert(de.diagnostics.count == 0);

    TLPrint(stderr, &tl);
    DEPrint(stderr, &de);

    // assert(((Token*)ListGet(&tl.tokens, 0))->kind == TK_INT);

    Ast ast = AstNew();
    if (!AstIsValid(&ast)) {
        fprintf(stderr, "<invalid AST in main()>\n");
        return 1;
    }

    Parser parser = ParserNew(&source, &ast, &de, &tl);
    if (!ParserIsValid(&parser)) {
        fprintf(stderr, "<invalid parser in main()>\n");
        return 1;
    }

    bool parseSuccess = false;
    Parse(&parser, &parseSuccess);

    DEPrint(stderr, &de);
    printf("Expr Count: %zu\n", parser.ast->exprs.count);

    AstPrinter astPrinter = AstPrinterNew(&source, &ast);
    AstPrintExpr(&astPrinter, 5);

    return 0;
}