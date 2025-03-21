//
// Created by adlyq on 25-3-21.
//

#include "ring_buffer.h"

#include <stddef.h>

bufferType* RingBuffer_get(struct RingBuffer* rb) {
    if (rb->cursor_tail == rb->cursor_head && !rb->isFull)
        return NULL;

    bufferType* data = &rb->buffer[rb->cursor_tail++];
    rb->cursor_tail %= BUFFER_SIZE;
    rb->cursor_head == rb->cursor_tail && ((rb->isFull = false));
    return data;
}

bool RingBuffer_put(struct RingBuffer* rb, const bufferType data) {
    if (rb->cursor_head == rb->cursor_tail && rb->isFull)
        return false;

    rb->buffer[rb->cursor_head++] = data;
    rb->cursor_head %= BUFFER_SIZE;
    rb->cursor_head == rb->cursor_tail && ((rb->isFull = true));
    return true;
}

bool RingBuffer_putPtr(struct RingBuffer* rb, const bufferType* data) {
    return RingBuffer_put(rb, *data);
}

u16 RingBuffer_size(const struct RingBuffer* rb) {
    if (rb->cursor_tail == rb->cursor_head)
        return  rb->isFull ? BUFFER_SIZE : 0;

    if (rb->cursor_head > rb->cursor_tail)
        return rb->cursor_head - rb->cursor_tail;

    return BUFFER_SIZE - (rb->cursor_tail - rb->cursor_head);
}