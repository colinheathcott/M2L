#include "source.h"
#include <string.h>
#define STATIC_PATH_NAME "<static_data>"

// -------------------------------------------------------------------------- //
// MARK: Source
// -------------------------------------------------------------------------- //

Source Source_New_From_Data(const char *data) {
    return (Source) {
        .data = data,
        .path = STATIC_PATH_NAME,
        .length = strlen(data)
    };
}

bool Substring_Is_Null(const Substring *str) {
    return str == NULL || str->length == 0 || str->data == NULL;
}

// -------------------------------------------------------------------------- //
// MARK: Substring
// -------------------------------------------------------------------------- //

Substring Source_Substring(const Source *src, const Span *span) {
    if (!src || !span) return NULL_SUBSTRING;

    // Check if the span is in bounds and return NULL if not.
    if (span->offset > src->length || span->offset + span->length > src->length)
        return NULL_SUBSTRING;
    
    const char *startptr = src->data + span->offset;
    return (Substring) {
        .data = startptr,
        .length = span->length
    };
}