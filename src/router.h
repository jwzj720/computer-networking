#ifndef ROUTER_H
#define ROUTER_H


#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "read.h"

int router(Packet* packet, uint8_t device_addr);

#endif /*ROUTER_H*/