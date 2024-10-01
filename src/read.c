#include <pigpiod_if2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#include "read.h"


#define MAX_BITS 700
#define BIT_COUNT 7

typedef struct {
    uint32_t READRATE;
    uint32_t ptime;
    uint32_t tick1;
    int rateset;
    int counter;
    int values;
    int run;
    char* data;
} ReadData;

// Want to create 2d array. Parent array contains subarrays of bit values, Maximum some number of bits.


void _callback(int pi, unsigned gpio, unsigned level, uint32_t tick, void* user) // added userdata for struct
{
    ReadData* rd = (ReadData*)user;
    //printf("READRATE: %"PRIu32"\n",rd->READRATE);
    //printf("Tick: %"PRIu32"\n",rd->tick);

    if (!(rd->rateset))
    {
        rd->tick1 = tick;
        rd->rateset++;
    }
    else if (rd->rateset == 1)
    {
        rd->READRATE = tick - rd->tick1;
        rd->ptime = tick;
        rd->rateset++;
    }
    else
    {
        //printf("timegap: %"PRIu32"\n",tick-ptime);
        // if the difference since the last tick is significantly less than expected readtime,
        // it is probably one that we want to ignore.
        if (((tick - rd->ptime) + (rd->READRATE * 0.25) > rd->READRATE))
        {
            printf("Level: %u\n", level);
            rd->data[rd->counter] = level ? '1' : '0';
            rd->values += level ? 1 : 0; // Add the level value to the values counter
            rd->counter++;
            rd->ptime = tick;
        }
    }

    if (rd->counter % BIT_COUNT == 0 && rd->counter > 0) //Every x values...
    {
        printf("Counter is zeroed\n");
        printf("Values: %d\n", rd->values);
        if (rd->values == BIT_COUNT) // If values equal BIT_COUNT, all bits are '1'
        {
            rd->data[rd->counter] = '\0'; 
            rd->run = 0; // Stop the loop
        }
        rd->values = 0; // Reset the values counter
    }

    if (rd->counter >= MAX_BITS)
    {
        rd->run = 0; 
        rd->data[MAX_BITS - 1] = '\0'; // Ensure null termination
    }
}

char* read_bits(int GPIO_SEND, int GPIO_RECEIVE)
{
    // Create and initialize the ReadData struct
    ReadData rd;
    rd.READRATE = 0;
    rd.ptime = 0;
    rd.tick1 = 0;
    rd.rateset = 0;
    rd.counter = 0;
    rd.values = 0;
    rd.run = 1;
    rd.data = malloc(MAX_BITS * sizeof(char) + 1);
    if (rd.data == NULL)
    {
        fprintf(stderr, "Couldn't allocate memory for data\n");
        return NULL;
    }
    memset(rd.data, 0, sizeof(char) * (MAX_BITS + 1));

    int pinit = pigpio_start(NULL, NULL);

    if (pinit < 0)
    {
        fprintf(stderr, "Didn't initialize pigpio library\n");
        free(rd.data);
        return NULL; // Return NULL to indicate failure
    }
    else
    {
        printf("Initialization success\n");
    }

    // Set GPIO modes
    if (set_mode(pinit, GPIO_SEND, PI_OUTPUT) != 0)
    {
        fprintf(stderr, "Failed to set GPIO_SEND mode\n");
        pigpio_stop(pinit);
        free(rd.data);
        return NULL;
    }

    if (set_mode(pinit, GPIO_RECEIVE, PI_INPUT) != 0)
    {
        fprintf(stderr, "Failed to set GPIO_RECEIVE mode\n");
        pigpio_stop(pinit);
        free(rd.data);
        return NULL;
    }

    // Register the callback with user data
    int id = callback_ex(pinit, GPIO_RECEIVE, EITHER_EDGE, _callback, &rd);
    if (id < 0)
    {
        fprintf(stderr, "Failed to set callback\n");
        pigpio_stop(pinit);
        free(rd.data);
        return NULL;
    }
    printf("Callback ID: %d\n", id);

    // Wait until the reading is done
    while (rd.run)
    {
        usleep(1000); // changed to a sleep to reduce CPU usage
    }

    // Clean up
    callback_cancel(id);
    pigpio_stop(pinit);

    // Return the received data
    return rd.data;
}