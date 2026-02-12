#define M2L_TEST_IMPL

// Test headers
#include "test.h"
#include "testparser.h"

// Lib headers
#include "../src/parsing/parser.h"
#include "../src/parsing/expr.h"
#include "../src/parsing/printer.h"

void RunParserTests() {
    #define X(name) Test##name();
    TESTS
    #undef X
}

TEST(Call) {
    TestContext tctx = BEGIN("parse call");

    //
    // ---------------------- [[ PROCESSING ]] ----------------------
    //
    Context ctx = ContextNew("add(lhs: 1, rhs: 2)");
    ContextScan(&ctx);

    Parser parser = ParserNew(&ctx.source, &ctx.ast, &ctx.de, &ctx.tl);
    ExprId id = expression(&parser);
    AstPrinter astPrinter = AstPrinterNew(&ctx.source, &ctx.ast);

    ListDumpInfo(stderr, &ctx.ast.args);
    DEPrint(stderr, &ctx.de);
    AstPrintExpr(&astPrinter, id);

    //
    // ------------------------ [[ CHECKS ]] ------------------------
    //
    Expression *expr = AstExprGet(&ctx.ast, id);
    CHECK(tctx, expr, "invalid expr");
    CHECK(tctx, expr->kind == EXPR_CALL, "not a call");
    CHECK(tctx, expr->data.exprCall.argc == 2, "!= 2 arguments");

    END(tctx)
}
