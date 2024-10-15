#ifndef SEND_H
#define SEND_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <pigpiod_if2.h>
#include <string.h>

#define BAUD_RATE 100
int send_bytes(uint8_t *packet, size_t packet_size, int out_pin, int pi, int send_rate);
#endif /*SEND_H*/
