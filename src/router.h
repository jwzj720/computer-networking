#ifndef ROUTER_H
#define ROUTER_H


#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "read.h"

#define GPIO_SEND_1 27
#define GPIO_RECEIVE_1 26
#define GPIO_SEND_2 25
#define GPIO_RECEIVE_2 24
#define GPIO_SEND_3 23
#define GPIO_RECEIVE_3 22

struct User { /* address book: */
    struct User *next; /* next entry in chain */
    uint8_t ID; /* Id described in message */
    int GPIO_OUT; /* Port to write out packets to */
    int GPIO_IN; /* Port Receiving from */
};



#endif /*ROUTER_H*/
