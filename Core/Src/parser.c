#include "parser.h"

void parser_init(parser_t *p)
{
    p->state = WAIT_START;
    p->length = 0;
    p->cmd = 0;
    p->index = 0;
    p->checksum = 0;
}

bool parser_process_byte(parser_t *p, uint8_t byte, command_t *out_cmd)
{
    *out_cmd = CMD_NONE;

    switch (p->state)
    {
        case WAIT_START:
            if (byte == 0xAA)
            {
                p->state = READ_LEN;
            }
            break;

        case READ_LEN:
            if (byte == 0 || byte > 64) //guard for byte sizes
            {
                p->state = WAIT_START;
                break;
            }

            p->length = byte;
            p->index = 0;
            p->checksum = byte;
            p->state = READ_CMD;
            break;

        case READ_CMD:
            p->cmd = byte;
            p->checksum ^= byte;
            p->state = (p->length > 1) ? READ_DATA : READ_CHECKSUM; //After reading the command byte, do I expect payload data next, or do I go straight to checksum?
            break;

        case READ_DATA:
            p->data[p->index++] = byte;
            p->checksum ^= byte;

            if (p->index >= (p->length - 1))
            {
                p->state = READ_CHECKSUM;
            }
            break;

        case READ_CHECKSUM:
            if (p->checksum == byte)
            {
                p->state = WAIT_END;
            }
            else
            {
                p->state = WAIT_START;
            }
            break;

        case WAIT_END:
            if (byte == 0x55)
            {
                switch (p->cmd)
                {
                    case 0x01:
                        *out_cmd = CMD_LED_ON;
                        p->state = WAIT_START;
                        return true;

                    case 0x02:
                        *out_cmd = CMD_LED_OFF;
                        p->state = WAIT_START;
                        return true;

                    case 0x03:
                        *out_cmd = CMD_LED_TOGGLE;
                        p->state = WAIT_START;
                        return true;

                    default:
                        break;
                }
            }

            p->state = WAIT_START;
            break;
    }

    return false;
}
