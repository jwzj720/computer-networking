#ifndef LINK_SEND_H
#define LINK_SEND_H

#include <stdlib.h>
#include <stdio.h>
#include <pigpiod_if2.h>
#include <string.h>
#include <inttypes.h>

#include "read.h"
#include "send.h"
#include "text_7bit.h"

#define GPIO_SEND 23
#define GPIO_RECEIVE 26

int send_to_file();

#endif /*SEND_H*/
