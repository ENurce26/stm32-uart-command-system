#ifndef COMMAND_QUEUE_H
#define COMMAND_QUEUE_H

#include <stdbool.h>
#include <stdint.h>
#include "parser.h"

#define COMMAND_QUEUE_SIZE 8

typedef struct
{
    command_t buffer[COMMAND_QUEUE_SIZE];
    uint8_t head;
    uint8_t tail;
} command_queue_t;

void command_queue_init(command_queue_t *q);
bool command_queue_push(command_queue_t *q, command_t cmd);
bool command_queue_pop(command_queue_t *q, command_t *cmd);

#endif
