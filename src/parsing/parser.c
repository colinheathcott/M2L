#include "parser.h"
#include "../common/list.h"
#include "../scanning/token.h"
#include "ast.h"
#include "expr.h"
#include <assert.h>
#include <stdint.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define POISON(id) if ((id) == NULL_AST_ID) return NULL_AST_ID

#define STATUS(parser, msg)                                                    \
    fprintf(stderr, "STATUS: '%s'", (msg));                                    \
    fprintf(stderr, "\n  GET(0): %s",TokenKindAsString(get((parser),0)->kind));\
    fprintf(stderr, "\n  GET(1): %s",TokenKindAsString(get((parser),1)->kind));\
    fprintf(stderr, "\n");

#ifdef LOG_PARSER
#define LOG(fmt, ...) fprintf(stderr, fmt __VA_OPT__(,) __VA_ARGS__)
#else
#define LOG(fmt, ...) ((void)0)
#endif

// -------------------------------------------------------------------------- //
// MARK: Converters
// -------------------------------------------------------------------------- //

// Note: In the future, I may write a custom method to replace the length + 1
// position with zero while the conversion is taking place, to trick strtoll
// into thinking there is a null terminator, then replace it with the original
// value when its done. This would be to avoid heap allocations.
bool convertIntLiteral(const Span *span, int64_t *valout, Diagnostic *errout) {
    char *endptr;
    int errno = 0;

    // Get the substring from the span
    const Substring str = SpanSubstring(span);
    if (SubstringIsNull(&str)) {
        const Diagnostic diag = DiagNew(
            ERR_INTERNAL,
            "integer token in convertIntLiteral() yielded a null substring",
            (DiagReport) { *span, "" }
        );
        // Silly workaround to avoid making Diagnostic members non-const.
        memcpy(errout, &diag, sizeof(Diagnostic));
        return false;
    }

    // Allocate the substring
    const char *allocString = SubstringAlloc(&str);
    if (!allocString) {
        const Diagnostic diag = DiagNew(
            ERR_INTERNAL,
            "integer token in convertIntLiteral() failed to allocate substring",
            (DiagReport) { *span, "this span failed on its allocation attempt" }
        );
        // Silly workaround to avoid making Diagnostic members non-const.
        memcpy(errout, &diag, sizeof(Diagnostic));
        return false;
    }

    // Attempt to do the conversion
    long long result = strtoll(allocString, &endptr, 10);

    // Check for errors
    if (endptr == allocString) {
        // No digits were in the string
        const Diagnostic diag = DiagNew(
            ERR_INTERNAL,
            "integer token in convertIntLiteral() has span with no digits!",
            (DiagReport) { *span, "" }
        );
        // Silly workaround to avoid making Diagnostic members non-const.
        memcpy(errout, &diag, sizeof(Diagnostic));
        return false;
    } else if (*endptr != 0) {
        // Invalid characters in the string
        const Diagnostic diag = DiagNew(
            ERR_INTERNAL,
            "integer token in convertIntLiteral() has span with non-digits!",
            (DiagReport) { *span, "" }
        );
        // Silly workaround to avoid making Diagnostic members non-const.
        memcpy(errout, &diag, sizeof(Diagnostic));
        return false;
    } else {
        // Everything worked
        *valout = (int64_t)result;
        return true;
    }
}

bool convertFloatLiteral(const Span *span, double *valout, Diagnostic *errout) {
    char *endptr;
    int errno = 0;

    // Get the substring from the span
    const Substring str = SpanSubstring(span);
    if (SubstringIsNull(&str)) {
        const Diagnostic diag = DiagNew(
            ERR_INTERNAL,
            "float token in convertFloatLiteral() yielded a null substring",
            (DiagReport) { *span, "" }
        );
        // Silly workaround to avoid making Diagnostic members non-const.
        memcpy(errout, &diag, sizeof(Diagnostic));
        return false;
    }

    // Allocate the substring
    const char *allocString = SubstringAlloc(&str);
    if (!allocString) {
        const Diagnostic diag = DiagNew(
            ERR_INTERNAL,
            "float token in convertFloatLiteral()"
            "failed to allocate substring",
            (DiagReport) { *span, "this span failed on its allocation attempt" }
        );
        // Silly workaround to avoid making Diagnostic members non-const.
        memcpy(errout, &diag, sizeof(Diagnostic));
        return false;
    }

    // Attempt to do the conversion
    double result = strtod(allocString, &endptr);

    // Check for errors
    if (endptr == allocString) {
        // No digits were in the string
        const Diagnostic diag = DiagNew(
            ERR_INTERNAL,
            "float token in convertFloatLiteral() has span with no digits!",
            (DiagReport) { *span, "" }
        );
        // Silly workaround to avoid making Diagnostic members non-const.
        memcpy(errout, &diag, sizeof(Diagnostic));
        return false;
    } else if (*endptr != 0) {
        // Invalid characters in the string
        const Diagnostic diag = DiagNew(
            ERR_INTERNAL,
            "float token in convertFloatLiteral() has span with non-digits!",
            (DiagReport) { *span, "" }
        );
        // Silly workaround to avoid making Diagnostic members non-const.
        memcpy(errout, &diag, sizeof(Diagnostic));
        return false;
    } else {
        // Everything worked
        *valout = result;
        return true;
    }
}

// -------------------------------------------------------------------------- //
// MARK: Parser Helpers
// -------------------------------------------------------------------------- //

// Returns the number of tokens in this parser's token list.
size_t count(const Parser *self) {
    return self->tokenList->tokens.count;
}

// The token pointer returned from this function is guaraunteed to not be `NULL`
Token *get(const Parser *self, size_t k) {
    if (self->cursor + k >= count(self)) {
        return ((Token *)ListBack(&self->tokenList->tokens));
    }
    return (Token *)ListGet(&self->tokenList->tokens, self->cursor + k);
}

// The token pointer returned from this function is guaraunteed to not be `NULL`
Token *getBack(const Parser *self, size_t k) {
    if (self->cursor <= k) {
        return ((Token *)ListFront(&self->tokenList->tokens));
    }
    return (Token *)ListGet(&self->tokenList->tokens, self->cursor - k);
}

// Advances the parser `k` tokens ahead. Will automatically prevent `cursor`
// from being greater than the token list count.
void next(Parser *self, size_t k) {
    // Prevent the cursor from overflowing the length of the token list
    if (self->cursor + k >= count(self)) {
        self->cursor = count(self);
        return;
    }
    self->cursor += k;
}

// Checks that the token at `get(k)` is equivalent to the `kind` provided.
// Will advance the parser `k` tokens in this case.
// If not, will push a diagnostic and use `what` in the error report.
bool expect(Parser *self, size_t k, TokenKind kind, const char *what) {
    Token *tk = get(self, k);

    if (tk->kind == kind) {
        next(self, 1);
        return true;
    }

    // Create a diagnostic
    const DiagReport report = (DiagReport) {
        .span = tk->span,
        .message = ""
    };

    // char buffer[64];
    // sprintf(buffer, "Expected `%s`", what);
    const Diagnostic diag = DiagNew(
        ERR_INVALID_CHAR,
        what,
        report
    );

    DEPush(self->diagEngine, &diag);
    return false;
}

// Skips tokens until an semicolon is found, or the EOF token is reached.
// Intended to be used to avoid causing problems when complicated parsers run
// into errors that could propagate throughout entire AST structures.
void recover(Parser *self) {
    while (get(self, 0)->kind != TK_EOF && get(self, 0)->kind != TK_SEMICOLON) {
        next(self, 1);
    }
}

// -------------------------------------------------------------------------- //
// MARK: Expression Parsing
// -------------------------------------------------------------------------- //

ExprId expression(Parser *self);

// MARK: expr: atom()

// Parses an atomic expression, most generally a literal of some kind.
// * Integer
// * Float
// * String
// * Symbol
// * Boolean
ExprId atom(Parser *self) {
    LOG("atom()\n");
    const TokenKind kind = get(self, 0)->kind;
    const Span      span = get(self, 0)->span;

    switch (kind) {

    // integer
    // ---------------------------- //
    case TK_INT: {
        LOG(". int\n");
        Diagnostic diag = {0};
        int64_t    value = 0;

        // Attempt the conversion here
        if (!convertIntLiteral(&span, &value, &diag)) {
            DEPush(self->diagEngine, &diag);
            break; // return null
        }

        const Expression expr = {
            .span = span,
            .kind = EXPR_INT,
            .data = { .exprInt = value }
        };

        // Advance and return
        next(self, 1);
        return AstExprPush(self->ast, &expr);
    }

    // float
    // ---------------------------- //
    case TK_FLOAT: {
        LOG(". float\n");
        Diagnostic diag  = {0};
        double     value = 0;

        // Attempt the conversion here
        if (!convertFloatLiteral(&span, &value, &diag)) {
            DEPush(self->diagEngine, &diag);
            break; // return null
        }

        const Expression expr = {
            .span = span,
            .kind = EXPR_FLOAT,
            .data = { .exprFloat = value }
        };

        // Advance and return
        next(self, 1);
        return AstExprPush(self->ast, &expr);
    }

    // string
    // ---------------------------- //
    case TK_STR: {
        LOG(". str\n");
        Substring substring = SpanSubstring(&span);
        if (SubstringIsNull(&substring)) {
            const Diagnostic diag = DiagNew(
                ERR_INTERNAL,
                "string token span yielded a null substring",
                (DiagReport) { span, "" }
            );
            DEPush(self->diagEngine, &diag);
            break; // return null
        }

        //
        // Update the span of the substring so that we exclude the `"`.
        //

        // Make sure it's actually long enough to do our chopping.
        if (substring.length < 2) {
            const Diagnostic diag = DiagNew(
                ERR_INTERNAL,
                "string token span yielded a substring shorter than 2 chars!",
                (DiagReport) { span, "strings should be at least `\"\"`" }
            );
            DEPush(self->diagEngine, &diag);
            break; // return null
        }

        // Make of a copy of this pointer so I can increment it without mutating
        // the original substring.
        const char *ptrcpy = substring.data;
        const Substring string = {
            .data = ptrcpy++,               // chop prefix  `"`
            .length = substring.length - 1, // chop postfix `"`
        };
        assert(string.length >= 2);

        const Expression expr = {
            .span = span,
            .kind = EXPR_STR,
            .data = { .exprString = string }
        };

        // Advance and return
        next(self, 1);
        return AstExprPush(self->ast, &expr);
    }

    // symbol
    // ---------------------------- //
    case TK_SYMBOL: {
        LOG(". symbol\n");
        const Substring symbol = SpanSubstring(&span);
        if (SubstringIsNull(&symbol)) {
            const Diagnostic diag = DiagNew(
                ERR_INTERNAL,
                "symbol token span yielded a null substring",
                (DiagReport) { span, "" }
            );
            DEPush(self->diagEngine, &diag);
            break; // return null
        }

        const Expression expr = {
            .span = span,
            .kind = EXPR_SYMBOL,
            .data = { .exprSymbol = symbol }
        };

        // Advance and return
        next(self, 1);
        return AstExprPush(self->ast, &expr);
    }

    // booleans
    // ---------------------------- //
    case TK_TRUE: {
        LOG(". true\n");
        const Expression expr = {
            .span = span,
            .kind = EXPR_BOOL,
            .data = { .exprBool = true }
        };

        // Advance and return
        next(self, 1);
        return AstExprPush(self->ast, &expr);
    }
    case TK_FALSE: {
        LOG(". false\n");
        const Expression expr = {
            .span = span,
            .kind = EXPR_BOOL,
            .data = { .exprBool = false }
        };

        // Advance and return
        next(self, 1);
        return AstExprPush(self->ast, &expr);
    }

    default: {
        LOG(". no atom found!\n");
        const Diagnostic diag = DiagNew(
            ERR_INVALID_SYNTAX,
            "expected an atom",
            (DiagReport) { span, "" }
        );
        DEPush(self->diagEngine, &diag);
        break; // return null
    }    // Add the null terminator

    }

    next(self, 1);
    return NULL_AST_ID;
}

// MARK: expr: call()

ExprId call(Parser *self) {
    LOG("call()\n");
    const TokenKind kind = get(self, 0)->kind;
    const Span startSpan = get(self, 0)->span;

    ExprId callee = atom(self);
    POISON(callee);

    //
    // Look for the opening of a function call
    //
    if (get(self, 0)->kind == TK_LPAR) {
        STATUS(self, "found fn call");
        size_t argc  = 0;
        size_t argid = self->ast->args.count;
        printf(". argid initialized to: %zu\n", argid);

        // Eat the LPAR
        next(self, 1);

        // This condition will skip the whole loop if the thing immediately
        // following the LPAR is an RPAR (i.e. the case where there is no args)
        bool parsing = get(self, 0)->kind != TK_RPAR;

        STATUS(self, "before entering loop");

        //
        // Parse arguments
        //
        while (parsing) {
            LOG(". parsing arg\n");
            STATUS(self, "after entering loop");

            bool   hasLabel = false;
            Substring label = NULL_SUBSTRING;
            const Span argSpan = get(self, 0)->span;

            //
            // Labeled Argument
            //
            if (get(self, 0)->kind == TK_SYMBOL
                && get(self, 1)->kind == TK_COLON
            ) {
                LOG(".. arg has a label\n");
                STATUS(self, "labeled argument");

                const Substring substring = SpanSubstring(&argSpan);
                if (SubstringIsNull(&substring)) {
                    fprintf(
                        stderr,
                        "<call(): null substring in argument label>\n"
                    );
                }

                // Eat the colon and go to what's after it
                next(self, 2);

                // Bullshit to work around const members of Substring
                memcpy(&label, &substring, sizeof(Substring));
                hasLabel = true;
            }

            //
            // Argument expression
            //
            const ExprId value = expression(self);
            STATUS(self, "after arg expr parse");

            // COMMA -> keep parsing
            if (get(self, 0)->kind == TK_COMMA) {
                next(self, 1);

            // RPAR -> quit parsing
            } else if (get(self, 0)->kind == TK_RPAR) {
                next(self, 1);
                parsing = false;

            // Anything else -> error
            } else {
                STATUS(self, "whoops, error!");
                const Diagnostic diag = DiagNew(
                    ERR_INVALID_SYNTAX,
                    "expected either `,` to continue arguments or `)` to end"
                    " function call",
                    (DiagReport) { get(self, 0)->span, "" }
                );
                DEPush(self->diagEngine, &diag);
                recover(self);
                return NULL_AST_ID;
            }

            // Don't add this arguemnt if it's invalid
            if (value == NULL_AST_ID) {
                STATUS(self, "null arg expr");
                continue;
            }

            // Compute the span of the argument
            const Span previousSpan = getBack(self, 1)->span;
            const Span span = SpanMerge(&argSpan, &previousSpan);

            printf("valueId: %zu\n", value);

            Argument arg = (Argument) {
                .span = span,
                .hasLabel = hasLabel,
                .label = label,
                .value = value,
            };

            // Push the argument to the list
            ListPush(&self->ast->args, &arg);
            argc++;
        } // end with cursor -> RPAR

        STATUS(self, "after loop");
        printf("!argid: %zu, !argc: %zu\n", argid, argc);

        // Put it all together
        const Span *endSpan = &get(self, 0)->span;
        //                     ^~~~~~~~~~~ this should point to the RPAR
        const Expression expr = {
            .span = SpanMerge(&startSpan, endSpan),
            .kind = EXPR_CALL,
            .data = { .exprCall =
                { .callee = callee, .argc = argc, .argid = argid } }
        };

        // Advance and return
        next(self, 1);
        return AstExprPush(self->ast, &expr);
    }

    //
    // If there is not a function call, then return the expr up the call stack
    //
    return callee;
}

// MARK: expr: postfix()

ExprId postfix(Parser *self) {
    const Span startSpan = get(self, 0)->span;
    ExprId operand = call(self);
    POISON(operand);

    //
    // Look for a postfix operator AFTER the operand is parsed
    //
    const char *op;
    const TokenKind kind = get(self, 0)->kind;
    switch (kind) {
    case TK_PLUS_PLUS:
        op = "++";
        break;
    case TK_MIN_MIN:
        op = "--";
        break;
    default:
        return operand;
    }

    //
    // Complete the expression
    //
    const Span endSpan = getBack(self, 1)->span;
    const Expression expr = {
        .span = SpanMerge(&startSpan, &endSpan),
        .kind = EXPR_POSTFIX,
        .data = { .exprUnary = { operand, op } }
    };

    // Advance and return
    next(self, 1);
    return AstExprPush(self->ast, &expr);
}

// MARK: expr: prefix()

ExprId prefix(Parser *self) {
    LOG("prefix()\n");
    const Span startSpan = get(self, 0)->span;
    const TokenKind kind = get(self, 0)->kind;

    //
    // Look for a postfix operator BEFORE the operand is parsed
    //
    const char *op;
    switch (kind) {
    case TK_PLUS_PLUS:
        LOG(". op: ++\n");
        op = "++";
        break;
    case TK_MIN_MIN:
        LOG(". op: --\n");
        op = "--";
        break;
    case TK_BANG:
        LOG(". op: !\n");
        op = "!";
        break;
    case TK_MIN:
        LOG(". op: -\n");
        op = "-";
        break;
    default:
        return postfix(self);
    }

    next(self, 1);

    //
    // Then parse the operand
    //
    ExprId operand = postfix(self);
    POISON(operand);
    LOG(". good operand\n");

    //
    // Complete the expression
    //
    const Span endSpan = getBack(self, 1)->span;
    const Expression expr = {
        .span = SpanMerge(&startSpan, &endSpan),
        .kind = EXPR_PREFIX,
        .data = { .exprUnary = { operand, op } }
    };

    // Advance and return
    // next(self, 1);
    return AstExprPush(self->ast, &expr);
}

// MARK: expr: factor()

ExprId factor(Parser *self) {
    LOG("factor()\n");
    const Span startSpan = get(self, 0)->span;

    const ExprId lhs = prefix(self);
    POISON(lhs);

    //
    // Look for a binary infix operator BEFORE the RHS is parsed
    //
    const char *op;
    switch (get(self, 0)->kind) {
    case TK_STAR:
        LOG(". op: *\n");
        op = "*";
        break;
    case TK_SLASH:
        LOG(". op: /\n");
        op = "/";
        break;
    default:
        return lhs;
    }

    next(self, 1);

    // Parse the RHS expression
    const ExprId rhs = expression(self);
    POISON(rhs);
    LOG(". good rhs\n");

    //
    // Complete the expression
    //
    const Span endSpan = getBack(self, 1)->span;
    const Expression expr = {
        .span = SpanMerge(&startSpan, &endSpan),
        .kind = EXPR_BINARY,
        .data = { .exprBinary = { lhs, rhs, op } }
    };

    // Advance and return
    // next(self, 1);
    return AstExprPush(self->ast, &expr);
}

// MARK: expr: term()

ExprId term(Parser *self) {
    LOG("term()\n");
    const Span startSpan = get(self, 0)->span;

    const ExprId lhs = factor(self);
    POISON(lhs);

    //
    // Look for a binary infix operator BEFORE the RHS is parsed
    //
    const char *op;
    switch (get(self, 0)->kind) {
    case TK_PLUS:
        LOG(". op: +\n");
        op = "+";
        break;
    case TK_MIN:
        LOG(". op: -\n");
        op = "-";
        break;
    default:
        return lhs;
    }

    next(self, 1);

    // Parse the RHS expression
    const ExprId rhs = expression(self);
    POISON(rhs);
    LOG(". good rhs\n");

    //
    // Complete the expression
    //
    const Span endSpan = getBack(self, 1)->span;
    const Expression expr = {
        .span = SpanMerge(&startSpan, &endSpan),
        .kind = EXPR_BINARY,
        .data = { .exprBinary = { lhs, rhs, op } }
    };

    // Advance and return
    // next(self, 1);
    return AstExprPush(self->ast, &expr);
}

// MARK: expr: comparison()

ExprId comparison(Parser *self) {
    LOG("comparison()\n");
    const Span startSpan = get(self, 0)->span;

    const ExprId lhs = term(self);
    POISON(lhs);

    //
    // Look for a binary infix operator BEFORE the RHS is parsed
    //
    const char *op;
    switch (get(self, 0)->kind) {
    case TK_LT:
        LOG(". op: <\n");
        op = "<";
        break;
    case TK_LT_EQ:
        LOG(". op: <=\n");
        op = "<=";
        break;
    case TK_GT:
        LOG(". op: >\n");
        op = ">";
        break;
    case TK_GT_EQ:
        LOG(". op: >=\n");
        op = ">=";
        break;
    default:
        return lhs;
    }

    next(self, 1);

    // Parse the RHS expression
    const ExprId rhs = expression(self);
    POISON(rhs);
    LOG(". good rhs\n");

    //
    // Complete the expression
    //
    const Span endSpan = getBack(self, 1)->span;
    const Expression expr = {
        .span = SpanMerge(&startSpan, &endSpan),
        .kind = EXPR_COMPARE,
        .data = { .exprBinary = { lhs, rhs, op } }
    };

    // Advance and return
    // next(self, 1);
    return AstExprPush(self->ast, &expr);
}

// MARK: expr: equality()

ExprId equality(Parser *self) {
    LOG("equality()\n");
    const Span startSpan = get(self, 0)->span;

    const ExprId lhs = comparison(self);
    POISON(lhs);

    //
    // Look for a binary infix operator BEFORE the RHS is parsed
    //
    const char *op;
    switch (get(self, 0)->kind) {
    case TK_EQ_EQ:
        LOG(". op: ==\n");
        op = "==";
        break;
    case TK_BANG_EQ:
        LOG(". op: !=\n");
        op = "!=";
        break;
    default:
        return lhs;
    }

    next(self, 1);

    // Parse the RHS expression
    const ExprId rhs = expression(self);
    POISON(rhs);
    LOG(". good rhs\n");

    //
    // Complete the expression
    //
    const Span endSpan = getBack(self, 1)->span;
    const Expression expr = {
        .span = SpanMerge(&startSpan, &endSpan),
        .kind = EXPR_EQUALITY,
        .data = { .exprBinary = { lhs, rhs, op } }
    };

    // Advance and return
    // next(self, 1);
    return AstExprPush(self->ast, &expr);
}

// MARK: expr: logicalOr()

ExprId logicalOr(Parser *self) {
    LOG("logicalOr()\n");
    const Span startSpan = get(self, 0)->span;

    const ExprId lhs = equality(self);
    POISON(lhs);

    //
    // Look for the operator and return if it isn't there
    //
    if (get(self, 0)->kind != TK_PIPE_PIPE) {
        return lhs;
    }
    const char *op = "||";
    next(self, 1);

    LOG(". op: ||\n");

    // Parse the RHS expression
    const ExprId rhs = expression(self);
    POISON(rhs);
    LOG(". good rhs\n");

    //
    // Complete the expression
    //
    const Span endSpan = getBack(self, 1)->span;
    const Expression expr = {
        .span = SpanMerge(&startSpan, &endSpan),
        .kind = EXPR_LOGICAL,
        .data = { .exprBinary = { lhs, rhs, op } }
    };

    // Advance and return
    // next(self, 1);
    return AstExprPush(self->ast, &expr);
}

// MARK: expr: logicalAnd()

ExprId logicalAnd(Parser *self) {
    LOG("logicalAnd()\n");
    const Span startSpan = get(self, 0)->span;

    const ExprId lhs = logicalOr(self);
    POISON(lhs);

    //
    // Look for the operator and return if it isn't there
    //
    if (get(self, 0)->kind != TK_AND_AND) {
        return lhs;
    }
    const char *op = "&&";
    next(self, 1);

    LOG(". op: &&\n");

    // Parse the RHS expression
    const ExprId rhs = expression(self);
    POISON(rhs);
    LOG(". good rhs\n");

    //
    // Complete the expression
    //
    const Span endSpan = getBack(self, 1)->span;
    const Expression expr = {
        .span = SpanMerge(&startSpan, &endSpan),
        .kind = EXPR_LOGICAL,
        .data = { .exprBinary = { lhs, rhs, op } }
    };

    // Advance and return
    // next(self, 1);
    return AstExprPush(self->ast, &expr);
}

// MARK: expr: assignment()

ExprId assignment(Parser *self) {
    LOG("assignment()\n");
    const Span startSpan = get(self, 0)->span;

    const ExprId assignee = logicalAnd(self);
    POISON(assignee);

    //
    // Look for a binary infix operator BEFORE the RHS is parsed
    //
    const char *op;
    switch (get(self, 0)->kind) {
    case TK_EQ:
        LOG(". op: =\n");
        op = "=";
        break;
    case TK_PLUS_EQ:
        LOG(". op: +=\n");
        op = "+=";
        break;
    case TK_MIN_EQ:
        LOG(". op: -=\n");
        op = "-=";
        break;
    case TK_STAR_EQ:
        LOG(". op: *=\n");
        op = "*=";
        break;
    case TK_SLASH_EQ:
        LOG(". op: /=\n");
        op = "/=";
        break;
    default:
        return assignee;
    }

    next(self, 1);

    // Parse the value expression
    const ExprId value = expression(self);
    POISON(value);
    LOG(". good value\n");

    //
    // Complete the expression
    //
    const Span endSpan = getBack(self, 1)->span;
    const Expression expr = {
        .span = SpanMerge(&startSpan, &endSpan),
        .kind = EXPR_ASSIGN,
        .data = { .exprBinary = { assignee, value, op } }
    };

    // Advance and return
    // next(self, 1);
    return AstExprPush(self->ast, &expr);
}

ExprId expression(Parser *self) { return assignment(self); }

// -------------------------------------------------------------------------- //
// MARK: Parser API
// -------------------------------------------------------------------------- //

Parser ParserNew(
    const Source *src,
    Ast *ast,
    DiagEngine *diagEngine,
    const TokenList *tokenList
) {
    if (!src
        || !ast
        || !diagEngine
        || !tokenList
        || tokenList->tokens.count == 0
    ) {
        return (Parser) {0};
    }

    return (Parser) {
        .src = src,
        .diagEngine = diagEngine,
        .tokenList = tokenList,
        .ast = ast,
        .cursor = 0,
    };
}

bool ParserIsValid(const Parser *self) {
    return (self
        && self->ast
        && self->diagEngine
        && self->tokenList
        && ListIsValid(&self->tokenList->tokens)
        && ListIsValid(&self->diagEngine->diagnostics)
        && AstIsValid(self->ast)
    );
}

void Parse(Parser *self, bool *success) {
    ExprId expr = expression(self);
    if (expr != NULL_AST_ID) {
        *success = false;
        return;
    }

    *success = true;
}
