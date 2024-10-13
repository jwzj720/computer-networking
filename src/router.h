#ifndef ROUTER_H
#define ROUTER_H

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "send.h"
#include "hamming.h"
#include "read.h"
#include "build_packet.h"
#include "objects.h"


#define ROUTER_ADDRESS 0x00

struct User* lookup(uint8_t id);
void* read_thread(void* arg);
void process_control_packet(struct Packet* packet, int gpio_in);
void process_application_packet(struct Packet* packet);
int relay(struct Packet* pack);


#endif /*ROUTER_H*/
