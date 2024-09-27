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

int main()
{
    char* result = read_bits(GPIO_SEND, GPIO_RECEIVE);
    printf("Binary received: %s\n",result);
    char* final = binary_to_text7(result);
    printf("Results: %s\n",final); //comment
    return 0;
}
