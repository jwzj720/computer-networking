#ifndef ROUTER_H
#define ROUTER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define MAX_INPUT_LENGTH 280
#define GPIO_SEND 27 //TODO: change this to be dynamically updated

int send_message(int pi);
uint8_t* text_to_bytes(size_t* length);
char* bytes_to_text(const uint8_t* bytes, size_t len);

#endif