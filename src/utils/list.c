#include "list.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#if GROWTH_FACTOR <= 1
#error GROWTH_FACTOR <= 1 will create infinite realloc loop!
#endif

static inline bool mul_will_overflow_sizet(size_t cap, size_t size) {
    return cap > SIZE_MAX / size;
}

// -------------------------------------------------------------------------- //
// MARK: Implementation
// -------------------------------------------------------------------------- //

List List_New(size_t size, size_t capacity) {
    // Make sure the sizes and capacities are valid
    if (capacity == 0 || size == 0
        || mul_will_overflow_sizet(capacity, size))
    {
        return NULL_LIST;
    }
    
    // Attempt allocate
    void *data = (void *)malloc(size * capacity);
    if (!data) return NULL_LIST;

    return (List) {
        .data = data,
        .capacity = capacity,
        .count = 0,
        .size = size
    };
}

void *List_Get(const List *self, size_t i) {
    if (!List_Is_Valid(self) || i >= self->count)
        return NULL;
    return (char *)self->data + (i * self->size);
}

List_Result List_Push(List *self, void *item) {
    if (!List_Is_Valid(self) || item == NULL)
        return LIST_RES_NULLPTR;
    List_Result res = LIST_RES_OK;
    
    //
    // Grow (if needed)
    //
    if (self->count >= self->capacity) {
        // Check for overflow
        if (mul_will_overflow_sizet(self->capacity, GROWTH_FACTOR))
            return LIST_RES_OVERFLOW;

        // Update the capacity and realloc
        size_t new_capacity = self->capacity * GROWTH_FACTOR;
        
        // Check for overflow again
        if (mul_will_overflow_sizet(new_capacity, self->size))
            return LIST_RES_OVERFLOW;

        // Reallocate
        void *new_data = realloc(self->data, new_capacity * self->size);
        
        // Check for errors with the allocation and update the List
        if (!new_data) return LIST_RES_ERR;
        self->data     = new_data;
        self->capacity = new_capacity;
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

bool List_Is_Valid(const List *self) {
    return self
        && self->data != NULL
        && self->capacity > 0
        && self->size > 0;
}

List_Result List_Free(List *self) {
    if (!List_Is_Valid(self))
        return LIST_RES_NULLPTR;
    free(self->data);
    *self = NULL_LIST;
    return LIST_RES_OK;
}