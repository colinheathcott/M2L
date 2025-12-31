#include "utils/list.h"
#include "utils/source.h"
#include "utils/ansi.h"
#include "utils/diag.h"
#include "scanning/token.h"
#include "scanning/scanner.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

// -------------------------------------------------------------------------- //
// MARK: Scanning
// -------------------------------------------------------------------------- //

typedef struct ScanningResult {
    bool isOk;
    TokenList list;
} ScanningResult;

#define ERR    (ScanningResult) {.isOk = false, 0 };
#define OK(tl) (ScanningResult) { .isOk = true, .list = (tl) };

ScanningResult scan(const Source *source, DiagEngine *diagEngine) {
    // Make a token list
    TokenList tokenList = TLNew();
    if (!ListIsValid(&tokenList.tokens))
        return ERR;
    
    // Make a new scanner
    Scanner scanner = ScannerNew(source, diagEngine, &tokenList);
    if (!ScannerIsValid(&scanner))
        return ERR;

    // Scan tokens
    ScannerScanSource(&scanner);
    ScannerDestroy(&scanner);
    return OK(tokenList);
}

#undef ERR
#undef OK

// -------------------------------------------------------------------------- //
// MARK: Main
// -------------------------------------------------------------------------- //

int main(int argc, char **argv) {
    InitConsoleColors();

    const Source source = SourceNewFromData("+ += ()");
    DiagEngine de = DENew();
    ScanningResult scanRes = scan(&source, &de);

    printf("%zu\n", scanRes.list.tokens.count);

    assert(scanRes.isOk);
    assert(scanRes.list.tokens.count == 5);

    assert(((Token*)ListGet(&scanRes.list.tokens, 0))->kind == TK_PLUS);
    assert(((Token*)ListGet(&scanRes.list.tokens, 1))->kind == TK_PLUS_EQ);
    assert(((Token*)ListGet(&scanRes.list.tokens, 2))->kind == TK_LPAR);
    assert(((Token*)ListGet(&scanRes.list.tokens, 3))->kind == TK_RPAR);
    assert(((Token*)ListGet(&scanRes.list.tokens, 4))->kind == TK_EOF);
    
    return 0;
}