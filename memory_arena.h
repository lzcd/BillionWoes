#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#define Byte 8
#define Kilobyte 1024
#define Megabyte 1048576

typedef struct MemoryArena
{
    int size_maximum_scalar;
    size_t size_maximum;
    size_t size_used;
    size_t count;
    void *data;
} MemoryArena;

MemoryArena arena_create()
{
    MemoryArena arena = {
        .size_maximum_scalar = 0,
        .size_maximum = 0,
        .size_used = 0,
        .count = 0,
        .data = NULL};
    return arena;
}

void arena_destroy(MemoryArena *arena)
{
    arena->size_maximum_scalar = 0;
    arena->size_maximum = 0;
    arena->size_used = 0;
    free(arena->data);
}

int arena_remaining(MemoryArena *arena)
{
    return arena->size_maximum - arena->size_used;
}

void *arena_append(MemoryArena *arena, size_t item_size)
{
    size_t minimum_required_size = arena->size_used + item_size;
    int maximum_scalar = arena->size_maximum_scalar;
    bool needs_resizing = false;
    while (minimum_required_size >= (size_t)pow((double)2, (double)maximum_scalar))
    {
        maximum_scalar += 1;
        needs_resizing = true;
    }

    if (needs_resizing)
    {
        void *existing_data = NULL;
        bool data_copy_required = arena->size_used > 0;
        if (data_copy_required)
        {
            existing_data = arena->data;
        };
        arena->size_maximum_scalar = maximum_scalar;
        arena->size_maximum = (size_t)pow((double)2, (double)maximum_scalar);
        arena->data = calloc(1, arena->size_maximum);
        if (data_copy_required)
        {
            memcpy(arena->data, existing_data, arena->size_used);
            free(existing_data);
        }
    }

    void *start = (void *)((size_t)arena->data + (size_t)arena->size_used);
    arena->size_used += item_size;
    arena->count++;
    assert(arena->size_used <= arena->size_maximum);
    return start;
}

void arena_clear(MemoryArena *arena)
{
    arena->size_used = 0;
}