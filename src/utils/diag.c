#include "diag.h"
#include "ansi.h"
#include "list.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// -------------------------------------------------------------------------- //
// MARK: Helpers
// -------------------------------------------------------------------------- //

static inline size_t countDigits(size_t n) {
    if (n == 0)
        return 1;

    int c = 0;
    while (n > 0) {
        n /= 10;
        c++; 
    }
    return c;
}

static DiagLevel getLevelFromIssue(DiagIssue issue) {
    switch (issue) {
    case WARN_INTERNAL: return DIAG_LEVEL_WARN;
    default: break;
    }
    return DIAG_LEVEL_ERROR;
}

// -------------------------------------------------------------------------- //
// MARK: Enums
// -------------------------------------------------------------------------- //

const char *DiagIssueStringified(DiagIssue self) {
    switch (self) {
    case ERR_INTERNAL:       return "internal compiler error";
    case WARN_INTERNAL:      return "internal compiler warning";

    case ERR_INVALID_CHAR:   return "invalid character";
    case ERR_INVALID_STRING: return "invalid string";
    }
    return "unknown error";
}

const char *DiagLevelStringified(DiagLevel self) {
    switch (self) {
    case DIAG_LEVEL_ERROR: return "error";
    case DIAG_LEVEL_WARN:  return "warning";
    case DIAG_LEVEL_INFO:  return "info";
    }
    return "unknown level";
}

// -------------------------------------------------------------------------- //
// MARK: Report Render
// -------------------------------------------------------------------------- //

void DiagReportRender(const DiagReport *self, const char *underlineColor) {
    const char *data = self->span.src->data;
    size_t fileLen = self->span.src->length;
    size_t gutterSize = 2 + countDigits(self->span.y);

    // Header
    printf("in ");
    COLORIZE(ANSI_COLOR_BLUE);
    printf(
        "%s:%zu:%zu:\n",
        self->span.src->path,
        self->span.y,
        self->span.x
    );
    COLORIZE(ANSI_RESET);

    //
    // Find start of the line containing span.offset
    // @(expect) assume invariant #1 here:
    //
    size_t start = 0;
    for (size_t i = self->span.offset; ; i--) {
        if (data[i] == '\n') {
            start = i + 1;
            break;
        }
        if (i == 0) {
            start = 0;
            break;
        }
    }

    //
    // Count how many lines the span touches
    //
    size_t line_count = 1;
    for (size_t i = self->span.offset;
         i < fileLen && i < self->span.offset + self->span.length;
         i++)
    {
        if (data[i] == '\n')
            line_count++;
    }

    //
    // Render lines
    //
    size_t index = start;
    for (size_t ln = 0; ln < line_count; ln++) {
        size_t eol = fileLen;

        // Print line number gutter
        COLORIZE(ANSI_COLOR_BLUE);
        printf(" %zu | ", self->span.y + ln);
        COLORIZE(ANSI_RESET);

        // Print source line
        for (size_t i = index; i < fileLen; i++) {
            char ch = data[i];
            printf("%c", ch);
            if (ch == '\n') {
                eol = i;
                break;
            }
        }

        printf("\n");

        // Print caret gutter
        COLORIZE(ANSI_COLOR_BLUE);
        printf("%*s| ", (int)gutterSize, "");
        COLORIZE(ANSI_RESET);

        bool first_line =
            index <= self->span.offset &&
            self->span.offset < eol;

        for (size_t i = index; i < eol; i++) {
            COLORIZE(underlineColor);
            if (first_line && i == self->span.offset)
                printf("^");
            else if (
                i >= self->span.offset &&
                i < self->span.offset + self->span.length
            )
                printf("~");
            else
                printf(" ");
            COLORIZE(ANSI_RESET);
        }

        // Append message on last line
        if (ln == line_count - 1)
            printf(" %s", self->message);

        printf("\n");

        // Advance to next line
        index = (eol < fileLen) ? eol + 1 : fileLen;
    }
}

// -------------------------------------------------------------------------- //
// MARK: Diagnostic Methods
// -------------------------------------------------------------------------- //

Diagnostic DiagNew(
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

void DiagRender(const Diagnostic *self) {
    const char *underlineColor;
    const char *foregroundColor;

    switch(self->level) {
    case DIAG_LEVEL_ERROR: {
        underlineColor  = ANSI_COLOR_RED;
        foregroundColor = ANSI_COLOR_RED;
        break;
    }
    case DIAG_LEVEL_WARN: {
        underlineColor  = ANSI_COLOR_YELLOW;
        foregroundColor = ANSI_COLOR_YELLOW;
        break;
    }
    default: {
        underlineColor  = ANSI_COLOR_BLUE;
        foregroundColor = ANSI_COLOR_BLUE;
        break;
    }
    }

    const char *issueName = DiagIssueStringified(self->issue);
    const char *levelName = DiagLevelStringified(self->level);

    COLORIZE(foregroundColor);
    printf("%s: ", levelName);
    COLORIZE(ANSI_RESET);
    printf("%s ", issueName);

    DiagReportRender(&self->report, underlineColor);
    
    COLORIZE(ANSI_COLOR_BLUE);
    printf("help: ");
    COLORIZE(ANSI_RESET);
    printf("%s\n", self->message);
}

// -------------------------------------------------------------------------- //
// MARK: Diagnostic Engine
// -------------------------------------------------------------------------- //

DiagEngine DENew() {
    List diagList = ListNew(sizeof(Diagnostic), INIT_DIAG_LIST_CAP);
    if (!ListIsValid(&diagList)) return (DiagEngine) {0};
    else return (DiagEngine) { diagList };
}

void DEPush(DiagEngine *engine, const Diagnostic *diag) {
    /* discard */ ListPush(&engine->diagList, diag);
}

void DEPrint(FILE *ioStream, const DiagEngine *self) {
    if (!self || !ioStream || !ListIsValid(&self->diagList)) {
        fprintf(stderr, "<invalid diag engine pointer or IO stream pointer>\n");
        return;
    }

    for (size_t i = 0; i < self->diagList.count; i++) {
        Diagnostic *diag = (Diagnostic *)ListGet(&self->diagList, i);
        if (!diag) {
            fprintf(ioStream, "<invalid diag pointer in list>\n");
            continue;
        }
        DiagRender(diag);
    }
}