#ifndef READ_H
#define READ_H

#include <pigpiod_if2.h>
#include <string.h>
#include <stdlib.h>
#include "objects.h"

#define BIT_COUNT 8
#define MAX_BYTES 50

struct ReadData* create_reader(int this_id);
void reset_reader(struct ReadData* rd);
void get_bit(int pi, unsigned gpio, unsigned level, uint32_t tick, void* user);
uint8_t* read_bits(struct ReadData* rd); /* An example function declaration */
struct Packet* data_to_packet(uint8_t* data);

#endif
