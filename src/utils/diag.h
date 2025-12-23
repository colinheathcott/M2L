#ifndef DIAG_H
#define DIAG_H

#include "source.h"
#include "list.h"

typedef enum DiagIssue {
    /* Internal errors */
    ERR_INTERNAL = 0,

    /* Lexical errors */
    ERR_INVALID_CHAR,
    ERR_INVALID_STRING,
} DiagIssue;

typedef enum DiagLevel {
    DIAG_LEVEL_ERROR,
    DIAG_LEVEL_WARN,
    DIAG_LEVEL_INFO,
} DiagLevel;

const char *DiagLevelStringified(DiagLevel self);
const char *DiagIssueStringified(DiagIssue self);

typedef struct DiagReport {
    Span source_code;
    const char *message;
} DiagReport;

typedef struct Diagnostic {
    const DiagIssue issue;
    const DiagLevel level;
    const char *message;
    DiagReport report;
} Diagnostic;

Diagnostic DiagNew(DiagIssue issue, const char *message,
    DiagReport report);

void DiagRender(const Diagnostic *diag);

typedef struct DiagEngine {
    List diagList;
} DiagEngine;

void DiagEmit(DiagEngine *engine, Diagnostic *diag);

#endif