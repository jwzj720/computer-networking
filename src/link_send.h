#ifndef LINK_RECEIVE_H
#define LINK_RECEIVE_H

#include <stdlib.h>
#include <stdio.h>
#include <pigpiod_if2.h>
#include <string.h>
#include <inttypes.h>

#include "read.h"
#include "send.h"
#include "text_6bit.h"
#include "text_7bit.h"

#define GPIO_SEND 23
#define GPIO_RECEIVE 26

int send_to_file();

#endif /*SEND_H*/