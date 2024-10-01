#ifndef READ_H
#define READ_H

#include <pigpiod_if2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#define MAX_BITS 700
#define BIT_COUNT 7

char* read_bits(int GPIO_SEND, int GPIO_RECEIVE); /* An example function declaration */

#endif