#include "source.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#define STATIC_PATH_NAME "<static_data>"

// -------------------------------------------------------------------------- //
// MARK: Span
// -------------------------------------------------------------------------- //

Substring SpanSubstring(const Span *span) {
    if (!span || !span->src ) return NULL_SUBSTRING;

    assert(span->offset + span->length <= span->src->length);

    // Check if the span is in bounds and return NULL if not.
    if (span->offset > span->src->length
        || span->offset + span->length > span->src->length)
        return NULL_SUBSTRING;

    const char *startptr = span->src->data + span->offset;
    return (Substring) {
        .data = startptr,
        .length = span->length
    };
}

size_t SpanEnd(const Span *span) {
    if (!span) {
        fprintf(stderr, "<NULL span pointer in SpanEnd>\n");
        return 0;
    }

    return span->offset + span->length;
}

Span SpanMerge(const Span *lhs, const Span *rhs) {
    if (!lhs || !rhs) {
        fprintf(stderr, "<NULL span pointers in SpanMerge>\n");
        return (Span) {0};
    }

    if (lhs->src != rhs->src) {
        fprintf(stderr, "<cannot merge spans of different sources>\n");
        return (Span) {0};
    }

    // Store whichever span comes "first" in the file.
    const Span *min = lhs->offset <= rhs->offset ? lhs : rhs;

    // Then compute the end of each span and choose the largest one.
    size_t lhsEnd = SpanEnd(lhs);
    size_t rhsEnd = SpanEnd(rhs);
    size_t end    = lhsEnd >= rhsEnd ? lhsEnd : rhsEnd;

    assert(end >= min->offset);

    // Then compute the length from that.
    size_t length = end - min->offset;

    return (Span) {
        .src = min->src,
        .offset = min->offset,
        .length = length,
        .x = min->x,
        .y = min->y,
    };
}

// -------------------------------------------------------------------------- //
// MARK: Source
// -------------------------------------------------------------------------- //

Source SourceNewFromData(const char *data) {
    return (Source) {
        .data = data,
        .path = STATIC_PATH_NAME,
        .length = strlen(data)
    };
}

bool SubstringIsNull(const Substring *str) {
    return str == NULL || str->length == 0 || str->data == NULL;
}

// -------------------------------------------------------------------------- //
// MARK: Substring
// -------------------------------------------------------------------------- //

void SubstringPrint(FILE *ioStream, const Substring *self) {
    if (!self || !ioStream || SubstringIsNull(self)) {
        fprintf(stderr, "<invalid IO stream pointer or substring>\n");
        return;
    }
    fwrite(self->data, 1, self->length, ioStream);
}

bool SubstringCmpSubstring(Substring *a, Substring *b) {
    if (SubstringIsNull(a) || SubstringIsNull(b))
        return false;

    // First check length
    if (a->length != b->length)
        return false;

    // Check each character
    for (size_t i = 0; i < b->length; i++)
        if (a->data[i] != b->data[i]) return false;

    return true;
}

bool SubstringCmpString(Substring *a, const char *b) {
    if (SubstringIsNull(a) || b == NULL)
        return false;

    // First check length
    const size_t blen = strlen(b);
    if (a->length != blen)
        return false;

    // Check each character
    for (size_t i = 0; i < blen; i++)
        if (a->data[i] != b[i]) return false;

    return true;
}

char *SubstringAlloc(const Substring *self) {
    if (!self || SubstringIsNull(self))
        return NULL;

    size_t allocatedLength   = self->length + 1;

    // Pre-allocate the space
    char *data = malloc(sizeof(char) * allocatedLength);
    if (!data) return NULL;

    // Copy data
    memcpy(data, self->data, self->length);

    // Add the null terminator
    data[self->length] = '\0';

    return data;
}
