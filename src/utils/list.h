#ifndef LIST_H
#define LIST_H

#include <stddef.h>
#include <stdbool.h>

#define GROWTH_FACTOR 2
#define NULL_LIST (List) {0}

// MARK: List_Result
/* Used to represent the result of some operation on a list.
 */
typedef enum List_Result {
    /* Successful, no issues. */
    LIST_RES_OK  = 0,
    /* Successful, however the list was reallocated in the process. */
    LIST_RES_REALLOC,
    /* Not successful, an invalid pointer or quantity was passed. */
    LIST_RES_NULLPTR,
    /* Not successful, quantities used would have resulted in an overflow. */
    LIST_RES_OVERFLOW,
    /* Not successful, error with allocating the memory. */
    LIST_RES_ERR,
} List_Result;

// MARK: List
/* Abstraction over an arena allocator backed growable list/vector. Uses `void*`
 * under the hood for generic effect.
 * - `data` the beginning pointer to the data allocated in memory.
 * - `capacity` the capacity of the list as is. Cannot be zero.
 * - `count` the number of elements in the list now. Remember when indexing that
 * this is always `i + 1`.
 * - `size` the size of each element in the list.
 * 
 * `List` uses the `List_Result` enum for many functions.
 * `List` takes pointers when appending, but will copy the actual bytes--items
 * have the same lifetime as the list itself, even if the original data you
 * appended is freed.
 */
typedef struct List {
    void * data;
    size_t capacity;
    size_t count;
    size_t size;
} List;

/* Creates a new list with the given item size and capacity.
 * - `capacity` cannot be zero!
 * - `size` cannot be zero!
 * Will fail only if either of the two prior invariants are violated, or if
 * there is an error with `malloc`, in which case the `List` will be `NULL`.
 * Remember to use `List_Is_Valid` after creating a list!
 */
List List_New(size_t size, size_t capacity);

/* Used to fetch the `i` th element from a list. This will return the memory 
 * address of that item. Always check for `NULL`.
 */
void *List_Get(const List *self, size_t i);

/* Used to push a new element to the list. This may grow the list. It can also
 * fail in the case of overflow or `realloc` error. Always check the result
 * against `LIST_RESULT_OK` or `LIST_RESULT_REALLOCATED`.
 */
List_Result List_Push(List *self, void *item);

/* Frees the list and poisons it by making it `NULL`.
 */
List_Result List_Free(List *self);

/* Checks if a list is valid, including checks for the actual pointer itself,
 * the data pointer, the size, and the capacity.
 */
bool List_Is_Valid(const List *self);

#endif