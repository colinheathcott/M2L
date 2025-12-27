#ifndef DIAG_H
#define DIAG_H

#include "source.h"
#include "list.h"

typedef enum DiagIssue {
    /* Internal errors */
    ERR_INTERNAL = 0,
    WARN_INTERNAL,

    /* Lexical errors */
    ERR_INVALID_CHAR,
    ERR_INVALID_STRING,
} DiagIssue;

typedef enum DiagLevel {
    DIAG_LEVEL_ERROR,
    DIAG_LEVEL_WARN,
    DIAG_LEVEL_INFO,
} DiagLevel;

const char *DiagIssueStringified(DiagIssue self);

typedef struct DiagReport {
    Span span;
    const char *message;
} DiagReport;

void DiagReportRender(const DiagReport *self, const char *underlineColor);

typedef struct Diagnostic {
    const DiagIssue issue;
    const DiagLevel level;
    const char *message;
    DiagReport report;
} Diagnostic;

Diagnostic DiagNew(DiagIssue issue, const char *message, DiagReport report);

void DiagRender(const Diagnostic *self);

typedef struct DiagEngine {
    List diagList;
} DiagEngine;

void DiagEmit(DiagEngine *engine, Diagnostic *diag);

#endif