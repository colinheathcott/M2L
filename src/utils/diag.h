#ifndef DIAG_H
#define DIAG_H

#include "source.h"
#include "list.h"

#define INIT_DIAG_LIST_CAP 16

// -------------------------------------------------------------------------- //
// MARK: Enums
// -------------------------------------------------------------------------- //

/* Represents some enumerated diagnostic issue that can be raised and displayed
 * to the user. Issues are either `Info`, `Warn`, or `Error` levels.
 */
typedef enum DiagIssue {
    /* Internal errors */
    ERR_INTERNAL = 0,
    WARN_INTERNAL,

    /* Lexical errors */
    ERR_INVALID_CHAR,
    ERR_INVALID_STRING,
} DiagIssue;

/* Represents the level of the diagnostic, only `DIAG_LEVEL_ERROR` is capable of 
 * aborting compilation.
 */
typedef enum DiagLevel {
    DIAG_LEVEL_ERROR,
    DIAG_LEVEL_WARN,
    DIAG_LEVEL_INFO,
} DiagLevel;

/* Returns a diagnostic issue as a string (to be printed to the console).
 */
const char *DiagIssueStringified(DiagIssue self);

// -------------------------------------------------------------------------- //
// MARK: Report
// -------------------------------------------------------------------------- //

/* Used to represent some specific part of source code that has an issue. 
 * When rendered, it will display all the lines intersecting the span, as well
 * as an underline and brief message about the underlined code.
 */
typedef struct DiagReport {
    Span span;
    const char *message;
} DiagReport;

/* Renders a diagnostic report to `stdout` via `printf()`. This operates on the
 * invariant that `self->span` is valid, has a valid `src` pointer, and that
 * span was created by the scanner such that it is impossible to backtrack
 * beyond the source code data.
 */
void DiagReportRender(const DiagReport *self, const char *underlineColor);

// -------------------------------------------------------------------------- //
// MARK: Diagnostic
// -------------------------------------------------------------------------- //

/* The unit for all diagnostic issues within the compiler. Represents one item
 * of incorrect or noteworthy code to be displayed to the user upon
 * compilation.
 *
 * In the future, diagnostics will have alternate members that are specific to
 * some issues, i.e. printing the function definition and prototype in cases 
 * where they do not match.
 */
typedef struct Diagnostic {
    const DiagIssue issue;
    const DiagLevel level;
    const char *message;
    DiagReport report;
} Diagnostic;

/* Creates a new diagnostic and initializes all members.
 */
Diagnostic DiagNew(DiagIssue issue, const char *message, DiagReport report);

/* Renders the diagnostic as a whole to `stdout` via `printf()`.
 */
void DiagRender(const Diagnostic *self);

// -------------------------------------------------------------------------- //
// MARK: Engine
// -------------------------------------------------------------------------- //

/* Used to keep track of all diagnostics. Right now, it is just a wrapper
 * around `List`, but in the future will have helpful methods to sort 
 * diagnostics and organize them.
 *
 * There should be one diagnostic engine per translation unit (for now).
 */
typedef struct DiagEngine {
    List diagList;
} DiagEngine;

DiagEngine DENew();

/* Pushes a new diagnostic to the list.
 */
void DEPush(DiagEngine *engine, const Diagnostic *diag);

#endif