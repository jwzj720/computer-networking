#ifndef ROUTER_H
#define ROUTER_H

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "send.h"
#include "encoding.h"
#include "read.h"
#include "build_packet.h"
#include "router.h"


#define NUM_GPIO_PAIRS 4
#define ROUTER_ADDRESS 0x00


struct User { /* address book: */
    struct User *next; /* next entry in chain */
    uint8_t ID; /* Id described in message */
    int GPIO_OUT; /* Port to write out packets to */
    int GPIO_IN; /* Port Receiving from */
};

struct GPIO_Pair {
    int gpio_in;
    int gpio_out;
};

struct GPIO_Pair gpio_pairs[NUM_GPIO_PAIRS] = {
    {26, 27}, /* p1r, p1t */
    {24, 25}, /* p2r, p2t */
    {22, 23}, /* p3r, p3t */
    {20, 21}  /* p4r, p4t */
};

struct User* lookup(uint8_t id);
void* read_thread(void* arg);
void process_control_packet(struct Packet* packet, int gpio_in);
void process_application_packet(struct Packet* packet);
int relay(struct Packet* pack);


#endif /*ROUTER_H*/
