#ifndef SOURCE_H
#define SOURCE_H
#include <stddef.h>
#include <stdbool.h>

#define NULL_SUBSTRING (Substring) { .data = NULL, .length = 0 }

// MARK: Span
/* Used to represent some amount of bytes in the original source code.
 * `offset` and `length` quantities are measured in bytes.
 * - `x` is the user-facing column number (nonzero).
 * - `y` is the user-facing line number (nonzero).
 */
typedef struct Span {
    const size_t offset;
    const size_t length;
    const size_t x;
    const size_t y;
} Span;

// MARK: Substring
/* A helper struct for storing substrings of source code. These are non-owning!
 * - `data` is a pointer to the first byte of the substring in the original
 * location in memory.
 * - `length` is how long the substring is, i.e. how many bytes the substring
 * is from the original `data` pointer.
 * 
 * Substrings are not null terminated.
 * Substrings can be null.
 */
typedef struct Substring {
    const char * data;
    const size_t length;
} Substring;

/* Used to check if a substring is null, in which case either or both of the
 * following will be true:
 * - `length == 0`
 * - `data == NULL`
 */
bool Substring_Is_Null(const Substring *str);

// MARK: Source
/* Contains the information for a single translation unit's source code,
 * including the source code in bytes, the path name, and the size of the
 * source code.
 *
 * Sources made from a static string (like for testing) have `NULL` path.
 */
typedef struct Source {
    const char * data;
    const char * path;
    const size_t length;
} Source;

/* Creates a new source from the provided static string.
 */
Source Source_New_From_Data(const char *data);

/* Returns a `Substring` of the bytes pointed to by `span`. Will return 
 * `NULL_SUBSTRING` if there is out of bounds access or null arguments.
 * Use `Substring_Is_Null()` to confirm.
 */
Substring Source_Substring(const Source *src, const Span *span);

#endif