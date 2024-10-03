#ifndef BUILD_PACKET_H
#define BUILD_PACKET_H


#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define ADDRESS_SIZE_BYTES 1 
#define DATA_LENGTH_SIZE_BYTES 2 

void build_packet(uint8_t device_addr, uint8_t receiver_addr, uint8_t *data, size_t data_size, uint8_t *packet);
void print_packet_debug(uint8_t *packet, size_t byte_size);
#endif /*BUILD_PACKET_H*/
