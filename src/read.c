#include <pigpiod_if2.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

uint32_t READRATE=0; //preset rate expected between bits.
uint32_t ptime;
uint32_t tick1;
int rateset = 0;

void call_back(int pi, unsigned gpio, unsigned level, uint32_t tick)
{
    printf("Tick: %"PRIu32"",tick);
    if (!rateset)
    {
        tick1 = tick;
        rateset++;
    }
    else if (rateset==1)
    {
        READRATE = tick-tick1;
        ptime = tick;
        rateset++;
    }
    else
    {
        // if the difference since the last tick is significantly less than expected readtime,
        // it is probably one that we want to ignore.
        if ((tick-ptime)+1 > READRATE)
        {
            printf("GPIO pin: %x | Level: %x\n",gpio,level);
            ptime = tick;
        }
    }   
}

int main()
{
    int GPIO_SEND = 23;
    int GPIO_RECEIVE = 26;
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
    printf("ID: %d\n",id);
    while(1)
    {  
        fflush(stdout); //Forces system to empty buffered prints.
    }
    return 0;
}
