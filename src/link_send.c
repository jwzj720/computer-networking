#include <stdlib.h>
#include <stdio.h>
#include <pigpiod_if2.h>
#include <string.h>
#include <inttypes.h>

#include "read.h"
#include "send.h"
#include "text_6bit.h"


int main()
{
    char* binary = prompt();
    send(binary);
    return 0;
}