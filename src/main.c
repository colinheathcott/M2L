#include "common/list.h"
#include "common/source.h"
#include "common/ansi.h"
#include "common/diag.h"
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

    const Source source = SourceNewFromData("let a = \"Hello, SMPL!");
    DiagEngine de = DENew();
    TokenList  tl = TLNew();
    
    if (!ListIsValid(&tl.tokens) || !ListIsValid(&de.diagnostics)) return 1;
    
    // Make a new scanner
    Scanner scanner = ScannerNew(&source, &de, &tl);
    if (!ScannerIsValid(&scanner)) return 1;

    // Scan tokens
    bool success = false;
    Scan(&scanner, &success);
    if (!success) {
        TLPrint(stderr, &tl);
        DEPrint(stderr, &de);
        return 1;
    }

    printf("%zu\n", tl.tokens.count);
    //assert(tl.tokens.count == 4);
    assert(de.diagnostics.count == 0);

    TLPrint(stderr, &tl);
    DEPrint(stderr, &de);

    // assert(((Token*)ListGet(&tl.tokens, 0))->kind == TK_SYMBOL);
    // assert(((Token*)ListGet(&tl.tokens, 1))->kind == TK_PLUS);
    // assert(((Token*)ListGet(&tl.tokens, 2))->kind == TK_SYMBOL);
    // assert(((Token*)ListGet(&tl.tokens, 3))->kind == TK_EOF);
    
    return 0;
}