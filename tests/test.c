#include "test.h"
#include "../src/scanning/scanner.h"
#include <string.h>
#include <stdlib.h>

Context ContextNew(const char *srcData) {
    return (Context) {
        .source = SourceNewFromData(srcData),
        .de = DENew(),
        .tl = TLNew(),
        .ast = AstNew()
    };
}

void ContextScan(Context *self) {
    // Make a new scanner
    Scanner scanner = ScannerNew(&self->source, &self->de, &self->tl);

    // Scan tokens
    bool _ = false;
    Scan(&scanner, &_);

    // Print output for debugging
    TLPrint(stderr, &self->tl);
    DEPrint(stderr, &self->de);
}
