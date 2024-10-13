#ifndef BUILD_PACKET_H
#define BUILD_PACKET_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "objects.h"

#define ADDRESS_SIZE_BYTES 1 
#define DATA_LENGTH_SIZE_BYTES 2 

size_t build_packet(struct Packet* send_pack, uint8_t *packet);
void print_packet_debug(uint8_t *packet, size_t byte_size);
void print_packet_binary(uint8_t packet[50]);

#endif /*BUILD_PACKET_H*/
