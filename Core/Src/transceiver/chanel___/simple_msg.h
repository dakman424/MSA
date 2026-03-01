#ifndef __SIMPLE_PROTOCOL__
#define __SIMPLE_PROTOCOL__
#include "stdint.h"

typedef enum {
    MSG_CMD = 0,
    MSG_DATA = 1
} protocol_message_type;

typedef struct protocol_message
{
    const uint8_t start;
    protocol_message_type type;
    uint32_t index;
    void *data;
    uint16_t len;
    uint32_t crc;
    const uint8_t end;
} protocol_message;

#endif