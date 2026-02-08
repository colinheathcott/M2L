#ifndef TEST_H
#define TEST_H

#include <stdbool.h>
#include <stdio.h>

#define TEST_ERR  2
#define TEST_PASS 0
#define TEST_FAIL 1

typedef struct TestContext {
    const char *testName;
    int testCode;
    int testPoint;
    int testPointFailure;
} TestContext;

#define BEGIN(name)                                                            \
    (TestContext) {                                                            \
        .testName = (name),                                                    \
        .testCode = TEST_PASS,                                                 \
        .testPoint = 1,                                                        \
        .testPointFailure = 0,                                                 \
    };           

#define END(ctx)                                                               \
    fprintf(stderr, "%s - COMPLETE\n  %i of %i CHECKS PASSED\n  POST: %i\n",   \
        (ctx).testName, (ctx).testPoint - (ctx).testPointFailure,              \
        (ctx).testPoint, (ctx).testCode);                                      \
    return (ctx).testCode;

#define FAIL(tn, tp, m)                                                        \
    fprintf(stderr, "%s - FAILED (%i) '%s'\n", (tn), (tp), (m))

#define CHECK(ctx, c, m)                                                       \
    (ctx).testPoint++;                                                         \
    if (!(c)) {                                                                \
        FAIL((ctx).testName, (ctx).testPoint, (m));                            \
        (ctx).testCode = TEST_FAIL;                                            \
        (ctx).testPointFailure++;                                              \
    }

// Lib headers
#include "../src/common/source.h"
#include "../src/common/diag.h"
#include "../src/scanning/token.h"
#include "../src/parsing/ast.h"

typedef struct Context {
    const Source source;
    DiagEngine de;
    TokenList  tl;
    Ast ast;
    bool isValid;
} Context;

Context scanString(const char *input);

#endif