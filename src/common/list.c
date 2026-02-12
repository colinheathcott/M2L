#include "list.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#if GROWTH_FACTOR <= 1
#error GROWTH_FACTOR <= 1 will create infinite realloc loop!
#endif

static inline bool mulWillOverflowSizet(size_t cap, size_t size) {
    return cap > SIZE_MAX / size;
}

// -------------------------------------------------------------------------- //
// MARK: Implementation
// -------------------------------------------------------------------------- //

List ListNew(size_t size, size_t capacity) {
    // Make sure the sizes and capacities are valid
    if (capacity == 0 || size == 0
        || mulWillOverflowSizet(capacity, size))
    {
        return NULL_LIST;
    }

    // Attempt allocate
    void *data = (void *)malloc(size * capacity);
    if (!data) {
        fprintf(stderr, "<ListNew(): allocation failure>\n");
        return NULL_LIST;
    }

    return (List) {
        .data = data,
        .capacity = capacity,
        .count = 0,
        .size = size
    };
}

void ListDumpInfo(FILE *handle, const List *self) {
    if (!handle || !self) {
        fprintf(
            stderr,
            "<ListDumpInfo(): invalid list or handle\n"
        );
        return;
    }
    fprintf(handle, "Dumping list info:\n");
    fprintf(handle, "  Size: %zu\n", self->size);
    fprintf(handle, "  Count: %zu\n", self->count);
    fprintf(handle, "  Capacity: %zu\n", self->capacity);
}

void *ListGet(const List *self, size_t i) {
    if (!ListIsValid(self) || i >= self->count) {
        fprintf(
            stderr,
            "<ListGet(): invalid list or %zu is out of bounds>\n", i
        );
        return NULL;
    }

    return (char *)self->data + (i * self->size);
}

void *ListFront(const List *self) {
    if (!ListIsValid(self) || self->count == 0) {
        fprintf(stderr, "<ListFront(): invalid list or count = 0>\n");
        return NULL;
    }
    return self->data;
}

void *ListBack(const List *self) {
    if (!ListIsValid(self) || self->count == 0) {
        fprintf(stderr, "<ListBack(): invalid list or count = 0>\n");
        return NULL;
    }
    return (char *)self->data + ((self->count - 1) * self->size);
}

ListResult ListPush(List *self, const void *item) {
    if (!ListIsValid(self) || item == NULL) {
        fprintf(stderr, "<ListPush(): null list ptr>\n");
        return LIST_RES_NULLPTR;
    }
    ListResult res = LIST_RES_OK;

    //
    // Grow (if needed)
    //
    if (self->count >= self->capacity) {
        fprintf(stderr, "<ListNew(): reallaction>\n");
        // Check for overflow
        if (mulWillOverflowSizet(self->capacity, GROWTH_FACTOR)) {
            fprintf(stderr, "<ListNew(): overflow (1)>\n");
            return LIST_RES_OVERFLOW;
        }


        // Update the capacity and realloc
        size_t newCapacity = self->capacity * GROWTH_FACTOR;

        // Check for overflow again
        if (mulWillOverflowSizet(newCapacity, self->size)) {
            fprintf(stderr, "<ListNew(): overflow (2)>\n");
            return LIST_RES_OVERFLOW;
        }

        // Reallocate
        void *newData = realloc(self->data, newCapacity * self->size);

        // Check for errors with the allocation and update the List
        if (!newData) {
            fprintf(stderr, "<ListNew(): allocation failure>\n");
            return LIST_RES_ERR;
        }
        self->data     = newData;
        self->capacity = newCapacity;
        res = LIST_RES_REALLOC;
    }

    // Now actually append the thing
    void *dest = (char *)self->data + (self->count * self->size);

    // Copy the item into the List
    memcpy(dest, item, self->size);
    self->count++;

    // Return the result of this allocation
    return res;
}

bool ListIsValid(const List *self) {
    return self
        && self->data != NULL
        && self->capacity > 0
        && self->size > 0;
}

ListResult ListFree(List *self) {
    if (!ListIsValid(self))
        return LIST_RES_NULLPTR;
    free(self->data);
    *self = NULL_LIST;
    return LIST_RES_OK;
}
