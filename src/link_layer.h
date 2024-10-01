#ifndef LINK_LAYER_H
#define LINK_LAYER_H

#include <stdlib.h>
#include <stdio.h>
#include <pigpiod_if2.h>
#include <string.h>
#include <inttypes.h>

#include "read.h"
#include "send.h"
#include "text_7bit.h"
#include "link_receive.h"
#include "link_send.h"

char* listen_message(int GPIO_SEND, int GPIO_RECEIVE); // this method will be used to open a listening port and wait for a message
// to be recieved and then return the message as a string
int send_message(char *bitstring); // this method will be used to send a message to another device


#endif /*LINK_LAYER_H*/
