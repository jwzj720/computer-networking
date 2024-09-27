#include <stdlib.h>
#include <stdio.h>
#include <pigpiod_if2.h>
#include <string.h>
#include <inttypes.h>

#include "read.h"
#include "send.h"
#include "text_6bit.h"

#define GPIO_SEND 23
#define GPIO_RECEIVE 26

int main()
{
    char* result = readBits(GPIO_SEND, GPIO_RECEIVE);
    //printf("Success: %d",result);
    //printf("Results: %"PRIu32"\n",*results); //comment
    return 0;
}
