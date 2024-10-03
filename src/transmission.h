#ifndef TRANSMISSION_H
#define TRANSMISSION_H

#include <stdlib.h>
#include <stdio.h>
#include <pigpiod_if2.h>
#include <string.h>
#include <inttypes.h>

#include "read.h"
#include "send.h"
#include "encoding.h"

#define GPIO_SEND 27
#define GPIO_RECEIVE 26

int send_to_file();
//int read_to_file(struct ReadData* rd);

#endif
