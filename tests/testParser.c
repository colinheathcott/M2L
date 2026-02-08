#define M2L_TEST_IMPL

// Test headers
#include "test.h"
#include "testParser.h"

// Lib headers
#include "../src/parsing/parser.h"
#include "../src/parsing/expr.h"
#include "../src/parsing/printer.h"

// Tests
int testCall();

void TestParserSuite() {
    testCall();
}

int testCall() {
    TestContext tctx = BEGIN("parse call");

    Context ctx = scanString("add(1, 2)");
    if (!ctx.isValid) return TEST_ERR;

    //
    // Setup parser
    //
    Parser parser = ParserNew(&ctx.source, &ctx.ast, &ctx.de, &ctx.tl);

    //
    // Parse input
    //
    ExprId id = expression(&parser);

    //
    // Print the input and check
    //
    AstPrinter astPrinter = AstPrinterNew(&ctx.source, &ctx.ast);
    AstPrintExpr(&astPrinter, id);

    Expression *expr = AstExprGet(&ctx.ast, id);
    CHECK(tctx, expr, "invalid expr");
    CHECK(tctx, expr->kind == EXPR_CALL, "not an call");
    CHECK(tctx, expr->data.exprCall.argc == 2, "!= 2 arguments");

    END(tctx)
}