#include "diag.h"
#include "ansi.h"
#include <stdio.h>

static DiagLevel getLevelFromIssue(DiagIssue issue) {
    switch (issue) {
    default: break;
    }
    return DIAG_LEVEL_ERROR;
}

const char *DiagLevelStringified(DiagLevel self) {
    switch (self) {
    case DIAG_LEVEL_WARN  : return "Warning";
    case DIAG_LEVEL_INFO  : return "Info";
    case DIAG_LEVEL_ERROR : break;
    }
    return "Error";
}

const char *DiagIssueStringified(DiagIssue self) {
    switch (self) {
    case ERR_INVALID_CHAR:   return "Invalid Character";
    case ERR_INVALID_STRING: return "Invalid String";
    case ERR_INTERNAL:       return "Internal Compiler Error";
    }
    return "Unknown Error";
}

Diagnostic DiagnosticNew(
    DiagIssue issue,
    const char *message,
    DiagReport report
) {
    return (Diagnostic) {
        .issue   = issue,
        .level   = getLevelFromIssue(issue),
        .message = message,
        .report  = report,
    };
}

void DiagnosticRender(const Diagnostic *diag) {
    printf("Error");
}

void DiagEmit(DiagEngine *engine, Diagnostic *diag) {
    ListPush(&engine->diagList, diag); /* discard */
}