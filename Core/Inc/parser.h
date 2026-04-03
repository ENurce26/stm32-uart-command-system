#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    WAIT_START,
    READ_LEN,
    READ_CMD,
    READ_DATA,
    READ_CHECKSUM,
    WAIT_END
} parser_state_t;

typedef enum {
    CMD_NONE = 0,
    CMD_LED_ON = 0x01,
    CMD_LED_OFF = 0x02,
    CMD_LED_TOGGLE = 0x03
} command_t;

typedef struct {
    parser_state_t state;
    uint8_t length;
    uint8_t cmd;
    uint8_t data[64];
    uint8_t index;
    uint8_t checksum;
} parser_t;

void parser_init(parser_t *p);
bool parser_process_byte(parser_t *p, uint8_t byte, command_t *out_cmd);

#endif
