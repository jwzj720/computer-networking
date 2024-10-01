#ifndef READ_H
#define READ_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <pigpiod_if2.h>
#include <string.h>
#include "transmission.h"

#define MAX_BITS 700
#define BIT_COUNT 7

char* read_bits(int GPIO_SEND, int GPIO_RECEIVE);

#endif