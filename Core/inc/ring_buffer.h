//
// Created by adlyq on 25-3-21.
//

#ifndef RING_BUFFER_H
#define RING_BUFFER_H
#include <stdbool.h>
#include <stdint.h>
#include "util.h"

#define BUFFER_SIZE 10
typedef int bufferType;

struct RingBuffer {
    bufferType buffer[BUFFER_SIZE];
    u16        cursor_head;
    u16        cursor_tail;
    bool       isFull;
};

/**
 * @brief get data from ring buffer
 * @param rb RingBuffer
 * @return pointer to data, NULL if empty
 */
bufferType* RingBuffer_get(struct RingBuffer* rb);

/**
 * @brief put data into ring buffer
 * @param rb RingBuffer
 * @param data
 * @return true if not full
 */
bool RingBuffer_put(struct RingBuffer* rb, bufferType data);

/**
 * @brief put data into ring buffer
 * @param rb RingBuffer
 * @param data data ptr
 * @return true if not full
 */
bool RingBuffer_putPtr(struct RingBuffer* rb, const bufferType* data);

/**
 * @brief get size of ring buffer
 * @param rb RingBuffer
 * @return size of ring buffer
 */
u16 RingBuffer_size(const struct RingBuffer* rb);
#endif //RING_BUFFER_H
