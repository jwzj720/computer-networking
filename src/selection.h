#ifndef SELECTION_H
#define SELECTION_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define NUM_APPS 2
#define NUM_ADDRESSES 3

int select_application();
int select_address(const char** rec_name);

#endif /* SELECTION_H */