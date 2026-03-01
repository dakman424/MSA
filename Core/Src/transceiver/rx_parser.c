#include "rx_parser.h"
#include "stdio.h"

#define BUFFER_LEN 512

static uint8_t RING_RX_BUFFER[BUFFER_LEN] = {0};
static uint16_t RING_RX_PTR = 0;

void rx_parser_push_byte(uint8_t *data) {
    if (data == nullptr) {
        return;
    }
    if (RING_RX_PTR > BUFFER_LEN - 1) {
        RING_RX_PTR = 0;
    } 
    RING_RX_BUFFER[RING_RX_PTR] = *data;
    ++RING_RX_PTR;
}
void rx_parser_pop_byte(uint8_t *data){
    if (data == nullptr) {
        return;
    }
    if (RING_RX_PTR == 0) {
        RING_RX_PTR = BUFFER_LEN - 1;
    }
    *data = RING_RX_BUFFER[RING_RX_PTR];
    --RING_RX_PTR;
}

void rx_parser_push_bytes_row(uint8_t *data, uint16_t len) {
   if (data == nullptr) {
    return;
   }
   while (len--)
   {
        rx_parser_push_byte((data + len));
   }
   return;  
}

void rx_parser_pop_bytes_row(uint8_t *data, uint16_t len) {
   if (data == nullptr) {
    return;
   }
   uint16_t ptr = 0;
   while (ptr < len)
   {
        rx_parser_pop_byte((data + ptr));
        ++ptr;
   }
   return;  
}