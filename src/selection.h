#ifndef SELECTION_H
#define SELECTION_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define NUM_APPS 2
#define NUM_ADDRESSES 3

#define GPIO_SEND 27
#define GPIO_RECEIVE 26

int select_application();
int select_address();

#endif
