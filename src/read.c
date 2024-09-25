#include <pigpiod_if2.h>
#include <stdio.h>
#include <stdlib.h>

uint32_t time = 0;

void call_back(int pi, unsigned gpio, unsigned level, uint32_t tick)
{
    if (tick>((time/2)+1))
    {
        time = tick;
        printf("GPIO pin: %x | Level: %x\n",gpio,level);

    }
}

int main()
{
    int GPIO_SEND = 23;
    int GPIO_RECEIVE = 20;
    int pinit = pigpio_start(NULL,NULL);

    if (pinit<0)
    {
        printf("failed start");
        return -1;
    }
    else
    {
        printf("initialization success\n");
    }

    int status = set_mode(pinit,GPIO_SEND,PI_OUTPUT);
    if (status==0)
    {
        printf("0 status received\n");
    }

    status = set_mode(pinit,GPIO_RECEIVE,PI_INPUT);
    if (status==0)
    {
        printf("0 status received\n");
    }
    int id = callback(pinit,GPIO_RECEIVE,EITHER_EDGE,call_back);
    while(1)
    {  
        fflush(stdout); //Forces system to empty buffered prints.
    }
    return 0;
}