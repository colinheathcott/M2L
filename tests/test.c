#include "test.h"
#include "../src/scanning/scanner.h"

Context scanString(const char *input) {
    const Source source = SourceNewFromData(input);
    DiagEngine de = DENew();
    TokenList  tl = TLNew();
    
    if (!ListIsValid(&tl.tokens) || !ListIsValid(&de.diagnostics))
        return (Context) { .isValid = false };
    
    // Make a new scanner
    Scanner scanner = ScannerNew(&source, &de, &tl);
    if (!ScannerIsValid(&scanner))
        return (Context) { .isValid = false };

    // Scan tokens
    bool scanSuccess = false;
    Scan(&scanner, &scanSuccess);
    if (!scanSuccess) {
        TLPrint(stderr, &tl);
        DEPrint(stderr, &de);
        return (Context) { .isValid = false };
    }

    TLPrint(stderr, &tl);
    DEPrint(stderr, &de);

    // assert(((Token*)ListGet(&tl.tokens, 0))->kind == TK_INT);

    Ast ast = AstNew();
    if (!AstIsValid(&ast)) {
        fprintf(stderr, "<invalid AST in main()>\n");
        return (Context) { .isValid = false };
    }

    return (Context) {
        .ast = ast,
        .de = de,
        .tl = tl,
        .source = source,
        .isValid = true,
    };
}