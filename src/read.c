#include <pigpiod_if2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#include "read.h"
#define MAX_BITS 600


uint32_t READRATE=0; //preset rate expected between bits.
uint32_t ptime;
uint32_t tick1;
int rateset = 0;
int counter = 0;
uint32_t values = 0;
// TODO: Conver this array of integers to a smaller array of uint6_t values. That way I set bits into each value,
// and then we just have to convert resulting uint6_t values to characters.
char* data;

int run=1;

// Want to create 2d array. Parent array contains subarrays of bit values, Maximum some number of bits. 

void _callback(int pi, unsigned gpio, unsigned level, uint32_t tick)
{
    // printf("READRATE: %"PRIu32"\n",READRATE);
    //printf("Tick: %"PRIu32"\n",tick);
    if (!rateset)
    {
        tick1 = tick;
        rateset++;
    }
    else if (rateset==1)
    {
        READRATE = tick-tick1;
	//READRATE = READRATE*2;
        ptime = tick;
        rateset++;
    }
    else
    {
        //printf("timegap: %"PRIu32"\n",tick-ptime);
        // if the difference since the last tick is significantly less than expected readtime,
        // it is probably one that we want to ignore.
        if (((tick-ptime)+(READRATE*.25) > READRATE) /*&& ((tick-ptime)-(READRATE*.25) < READRATE)*/)
        {
            //printf("Level: %x\n",level);
            data[counter] = ((int) level) ? "0" : "1";
            values += data[counter]; // Add the level value to the values counter
            counter++;
            ptime = tick;
        }
    }
    if (counter%6 ==0) //Every 6 values...
    {
	printf("Counter is zeroed\n");
	printf("Values: %x\n",values);
        if ((int) values==6) //If values is still 0, there have been 6 in a row.
        {
            counter= MAX_BITS; //Set the counter to end reading. 
        }
        values = 0; //Reached if values were greater than 0. 
    }
    if (counter == MAX_BITS)
    {
        run = 0; //Stops the infinite while loop
        data[MAX_BITS] = '\0';
    }
}

char* read_bits(int GPIO_SEND, int GPIO_RECEIVE)
{

	
    data = malloc(MAX_BITS*sizeof(char)+1);
    memset(data, 0, sizeof(char)*MAX_BITS);

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
    //Try to reset gpio to 0
    set_pull_up_down(pinit,GPIO_RECEIVE,PI_PUD_DOWN);

    int id = callback(pinit,GPIO_RECEIVE,EITHER_EDGE, _callback);
    printf("ID: %d\n",id);
    while(run)
    {  
        fflush(stdout); //Forces system to empty buffered prints.
    }

    return data;
}
