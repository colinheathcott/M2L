#ifndef SOURCE_H
#define SOURCE_H
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#define NULL_SUBSTRING (Substring) { .data = NULL, .length = 0 }

// -------------------------------------------------------------------------- //
// MARK: Source
// -------------------------------------------------------------------------- //

// Contains the information for a single translation unit's source code,
// including the source code in bytes, the path name, and the size of the
// source code.
// 
// Sources made from a static string (like for testing) have `NULL` path.
typedef struct Source {
    const char * data;
    const char * path;
    const size_t length;
} Source;

/* Creates a new source from the provided static string.
 */
Source SourceNewFromData(const char *data);

// -------------------------------------------------------------------------- //
// MARK: Substring
// -------------------------------------------------------------------------- //

// A helper struct for storing substrings of source code. These are non-owning!
// - `data` is a pointer to the first byte of the substring in the original
// location in memory.
// - `length` is how long the substring is, i.e. how many bytes the substring
// is from the original `data` pointer.
//  
// Substrings are not null terminated.
// Substrings can be null.
typedef struct Substring {
    const char * data;
    const size_t length;
} Substring;

// Used to check if a substring is null, in which case either or both of the
// following will be true:
// - `length == 0`
// - `data == NULL`
bool SubstringIsNull(const Substring *str);

// Compares a `Substring` with another `Substring`.
bool SubstringCmpString(Substring *a, const char *b);

// Compares a `Substring` with a `const char *`.
bool SubstringCmpSubstring(Substring *a, Substring *b);

// Prints the substring to the specific IO stream.
void SubstringPrint(FILE *ioStream, const Substring *self);

// Allocates the substring data on the heap by copying bytes from the source
// data and placing them into heap allocated member.
//
// The pointer returned is a null-terminated sequence of bytes.
char *SubstringAlloc(const Substring *self);

// -------------------------------------------------------------------------- //
// MARK: Span
// -------------------------------------------------------------------------- //

// Used to represent some amount of bytes in the original source code.
// `offset` and `length` quantities are measured in bytes.
// - `x` is the user-facing column number (nonzero).
// - `y` is the user-facing line number (nonzero).
typedef struct Span {
    const Source *src;
    const size_t offset;
    const size_t length;
    const size_t x;
    const size_t y;
} Span;

// Returns a `Substring` of the bytes pointed to by `span`. Will return 
// `NULL_SUBSTRING` if there is out of bounds access or null arguments.
// Use `SubstringIsNull()` to confirm.
Substring SpanSubstring(const Span *span);

// Maximally merges two spans. Uses the smaller of the two offsets and
// length that points to the largest end index of the two spans.
Span SpanMerge(const Span *lhs, const Span *rhs);

// Returns the end index of the span.
size_t SpanEnd(const Span *span);

#endif