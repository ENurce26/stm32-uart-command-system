#include "ring_buffer.h"

void ring_buffer_init(ring_buffer_t *rb)
{
    rb->head = 0;
    rb->tail = 0;
}

bool ring_buffer_is_empty(const ring_buffer_t *rb)
{
    return rb->head == rb->tail;
}

bool ring_buffer_is_full(const ring_buffer_t *rb)
{
    uint16_t next = (rb->head + 1) % UART_RX_BUFFER_SIZE;
    return next == rb->tail;
}

bool ring_buffer_push(ring_buffer_t *rb, uint8_t byte)
{
    uint16_t next = (rb->head + 1) % UART_RX_BUFFER_SIZE;

    if (next == rb->tail)
    {
        return false;
    }

    rb->buffer[rb->head] = byte;
    rb->head = next;
    return true;
}

bool ring_buffer_pop(ring_buffer_t *rb, uint8_t *byte)
{
    if (rb->head == rb->tail)
    {
        return false;
    }

    *byte = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % UART_RX_BUFFER_SIZE;
    return true;
}
