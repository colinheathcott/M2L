#include "source.h"
#include <string.h>
#define STATIC_PATH_NAME "<static_data>"

// -------------------------------------------------------------------------- //
// MARK: Span
// -------------------------------------------------------------------------- //

Substring SpanSubstring(const Span *span) {
    if (!span || !span->src ) return NULL_SUBSTRING;

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
    if (!self || !ioStream) {
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