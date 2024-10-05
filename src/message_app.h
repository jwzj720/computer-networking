#ifndef ROUTER_H
#define ROUTER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "hamming.h"
#include "build_packet.h"
#include "selection.h"

#define MAX_INPUT_LENGTH 280
#define GPIO_SEND 27 //TODO: change this to be dynamically updated

uint8_t* send_message(size_t* data_size);
void read_message(uint8_t* packet, size_t packet_len, size_t* decoded_len);
uint8_t* text_to_bytes(size_t* length);
char* bytes_to_text(const uint8_t* bytes, size_t len);

#endif
