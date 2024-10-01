#ifndef READ_H
#define READ_H

#include <pigpiod_if2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#define MAX_BITS 700
#define BIT_COUNT 7

struct ReadData{
    uint32_t READRATE;
    uint32_t ptime;
    uint32_t tick1;
    int rateset;
    int counter;
    int values;
    int run;
    char* data;
};

void get_bit(int pi, unsigned gpio, unsigned level, uint32_t tick, void* user);
char* read_bits(int GPIO_SEND, int GPIO_RECEIVE); /* An example function declaration */

#endif