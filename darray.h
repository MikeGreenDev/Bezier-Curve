#pragma once

/**
 *  NOTE: If you want to pass a darray to a function. Be sure to pass it as
 * an [type]** because [type]* is the darray; The extra * makes it a
 * "pointer"
 */

enum {
    DARRAY_MAX_SIZE,
    DARRAY_LENGTH,
    DARRAY_STRIDE,
    DARRAY_FIELD_LENGTH
};

void* _darray_create(unsigned long long length, unsigned long long stride);
void _darray_destroy(void* array);
void* _darray_resize(void* array);
void* _darray_shrink(void* array);

unsigned long long _darray_field_get(void* array, unsigned long long field);
void _darray_field_set(void* array, unsigned long long field,
                     unsigned long long value);

void* _darray_push(void* array, const void* valuePtr);
void _darray_pop(void* array, void* dest);

void* _darray_pop_at(void* array, unsigned long long idx, void* dest);
void* _darray_insert_at(void* array, unsigned long long idx, void* valuePtr);

#define DARRAY_DEFAULT_SIZE 1
#define DARRAY_DEFAULT_RESIZE_FACTOR 2

//====================== Define function wrappers ======================

/**
 *  Create a darray array. Initial size will be 1. For custom size look at
 * `darrayCreateReserve()`
 */
#define darrayCreate(type) _darray_create(DARRAY_DEFAULT_SIZE, sizeof(type));

/**
 *  Create a darray array with a custom initial length.
 *  So if you know you need 32 elements you should do darrayCreateReserve(32,
 * [type]). This saves on having to redarray_MALLOC a ton of times.
 */
#define darrayCreateReserve(length, type) _darray_create(length, sizeof(type));

/**
 *  Frees the darray array
 */
#define darrayDestroy(array) _darray_destroy(array);

/**
 *  Shrinks the darray array to it's length so no memory is being wasted.
 *  This does perform a reallocate.
 */
#define darrayShrink(array) _darray_shrink(array);

/**
 *  Push an element value to the darray array. Will automatically resize
 */
#define darrayPush(array, value)                                                 \
    {                                                                          \
        typeof(value) t = value;                                               \
        array = _darray_push(array, &t);                                         \
    }

/**
 *  Pop the last element value from the darray array.
 */
#define darrayPop(array, valuePtr) _darray_pop(array, valuePtr)

/**
 *  Insert an element value to the darray array at a certain index. Will
 * automatically resize
 */
#define darrayInsertAt(array, index, value)                                      \
    {                                                                          \
        typeof(value) temp = value;                                            \
        array = _darray_insert_at(array, index, &temp);                          \
    }

/**
 *  Pop the element value with the index from the darray array.
 */
#define darrayPopAt(array, index, value_ptr) _darray_pop_at(array, index, value_ptr)

//====================== QOL Functions Defined ======================

/**
 *  Clears the darray array.
 *  Does NOT do any reallocating. So the memory will still be darray_MALLOCed.
 */
#define darrayClear(array) _darray_field_set(array, DARRAY_LENGTH, 0)

/**
 *  Sets the length of the darray Array.
 *  Made to be a helper function for other darrayArray functions. Users be
 * careful.
 */
#define darrayLengthSet(array, value)                                            \
    _darray_field_set(array, DARRAY_LENGTH, value)

//====================== QOL Variables Defined ======================

/**
 *  Get the Max Size / Capacity of the darray Array
 */
#define darrayMaxSize(array) _darray_field_get(array, DARRAY_MAX_SIZE)

/**
 *  Get the Length of the darray Array
 */
#define darrayLength(array) _darray_field_get(array, DARRAY_LENGTH)

/**
 *  Get the Stride of the darray Array
 */
#define darrayStride(array) _darray_field_get(array, DARRAY_STRIDE)

#ifdef DARRAY_IMPLEMENTATION
#include <stdio.h>
#include <string.h>

#ifndef DARRAY_MALLOC
#include <stdlib.h>
#define DARRAY_MALLOC(size) malloc(size)
#endif

#ifndef DARRAY_FREE
#include <stdlib.h>
#define DARRAY_FREE(block, size) free(block)
#endif


void* _darray_create(unsigned long long length, unsigned long long stride) {
    // Like an html network header
    // Stores info
    unsigned long long header =
        DARRAY_FIELD_LENGTH * sizeof(unsigned long long);
    unsigned long long mix = (length * stride) + header;
    void* newArr = DARRAY_MALLOC(mix);
    // Set header info
    ((unsigned long long*)newArr)[DARRAY_MAX_SIZE] = length;
    ((unsigned long long*)newArr)[DARRAY_LENGTH] =
        0; // Length of current elements
    ((unsigned long long*)newArr)[DARRAY_STRIDE] = stride;
    // Move the array up so the user can access their elements immediately
    return ((void*)(((unsigned long long*)newArr) + DARRAY_FIELD_LENGTH));
}

void _darray_destroy(void* array) {
    unsigned long long* header =
        (unsigned long long*)array - DARRAY_FIELD_LENGTH;
    DARRAY_FREE(header, (darrayLength(array) * darrayStride(array)) + (sizeof(unsigned long long) * DARRAY_FIELD_LENGTH));
}

void* _darray_resize(void* array) {
    unsigned long long length = darrayLength(array);
    unsigned long long stride = darrayStride(array);
    void* temp =
        _darray_create(darrayMaxSize(array) * DARRAY_DEFAULT_RESIZE_FACTOR, stride);
    memcpy(temp, array, length * stride);
    _darray_destroy(array);
    darrayLengthSet(temp, length);
    return temp;
}

void* _darray_shrink(void* array) {
    unsigned long long length = darrayLength(array);
    unsigned long long stride = darrayStride(array);
    void* temp = _darray_create(length + 1, stride);
    memcpy(temp, array, length * stride);
    _darray_destroy(array);
    darrayLengthSet(temp, length);
    return temp;
}

unsigned long long _darray_field_get(void* array, unsigned long long field) {
    unsigned long long* header =
        (unsigned long long*)array - DARRAY_FIELD_LENGTH;
    return (header[field]);
}

void _darray_field_set(void* array, unsigned long long field,
                     unsigned long long value) {
    unsigned long long* header =
        (unsigned long long*)array - DARRAY_FIELD_LENGTH;
    header[field] = value;
}

void* _darray_push(void* array, const void* valuePtr) {
    unsigned long long length = darrayLength(array);
    unsigned long long stride = darrayStride(array);
    // printf("Length: %llu, MaxSize: %llu\n", length, darrayMaxSize(array));
    if (length >= darrayMaxSize(array)) {
        // printf("Resizing Array\n");
        array = _darray_resize(array);
    }
    unsigned long long idx = (unsigned long long)array;
    // Since length is One-based and array is Zero-based we don't have to add
    // one for the new element
    idx += length * stride;
    memcpy((void*)idx, valuePtr, stride);
    darrayLengthSet(array, length + 1);
    return array;
}

void* _darray_insert_at(void* array, unsigned long long idx, void* valuePtr) {
    unsigned long long length = darrayLength(array);
    unsigned long long stride = darrayStride(array);
    if (idx >= length) {
        fprintf(stderr, "darray ERROR: Index was more than array length");
        return array;
    }
    if (length >= darrayMaxSize(array)) {
        array = _darray_resize(array);
    }
    unsigned long long memIdx = (unsigned long long)array;

    // If idx isn't at the end move elements after it down one
    // Element after the index to move the afterbit to
    unsigned long long elementAfter = memIdx + ((idx + 1) * stride);
    unsigned long long afterbit = memIdx + (idx * stride);
    if (idx != length - 1) {
        memcpy((void*)elementAfter, (void*)afterbit, stride * (length - idx));
    }
    // Actually copy the idx value into the array
    memcpy((void*)(memIdx + (idx * stride)), valuePtr, stride);
    darrayLengthSet(array, length + 1);
    return array;
}

void _darray_pop(void* array, void* dest) {
    unsigned long long length = darrayLength(array);
    unsigned long long stride = darrayStride(array);
    unsigned long long idx = (unsigned long long)array;
    idx += (length - 1) * stride;
    memcpy(dest, (void*)idx, stride);
    darrayLengthSet(array, length - 1);
}

void* _darray_pop_at(void* array, unsigned long long idx, void* dest) {
    unsigned long long length = darrayLength(array);
    if (idx >= length) {
        fprintf(stderr, "darray ERROR: Index was more than array length");
        return array;
    }
    unsigned long long stride = darrayStride(array);
    unsigned long long memIdx = (unsigned long long)array;
    unsigned long long eleIdx = idx * stride;
    // Copy the element to the dest
    memcpy(dest, (void*)(memIdx + eleIdx), stride);

    // If idx isn't at the end move elements after it down one
    // Element after the index to move the afterbit to
    unsigned long long elementAfter = memIdx + ((idx + 1) * stride);
    unsigned long long afterbit = memIdx + (idx * stride);
    if (idx != length - 1) {
        memcpy((void*)afterbit, (void*)elementAfter, stride * (length - idx));
    }
    darrayLengthSet(array, length - 1);
    return array;
}
#endif
