#include "command_queue.h"

void command_queue_init(command_queue_t *q)
{
    q->head = 0;
    q->tail = 0;
}

bool command_queue_push(command_queue_t *q, command_t cmd)
{
    uint8_t next = (q->head + 1) % COMMAND_QUEUE_SIZE;

    if (next == q->tail)
    {
        return false;
    }

    q->buffer[q->head] = cmd;
    q->head = next;
    return true;
}

bool command_queue_pop(command_queue_t *q, command_t *cmd)
{
    if (q->head == q->tail)
    {
        return false;
    }

    *cmd = q->buffer[q->tail];
    q->tail = (q->tail + 1) % COMMAND_QUEUE_SIZE;
    return true;
}
