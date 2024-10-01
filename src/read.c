#include "read.h"

// Want to create 2d array. Parent array contains subarrays of bit values, Maximum some number of bits.


void get_bit(int pi, unsigned gpio, unsigned level, uint32_t tick, void* user) // added userdata for struct
{
    struct ReadData* rd = (struct ReadData*) user;
    printf("READRATE: %"PRIu32" ; ",rd->READRATE);
    printf("Tick: %"PRIu32" ; ",tick);
    printf("Rateset: %d ; ",rd->rateset);
    if(!(rd->run))
    {
	    printf("Bit shouldn't read\n");
        return;
    }
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
        // if the difference since the last tick is significantly less than expected readtime,
        // it is probably one that we want to ignore.
        if ((tick - rd->ptime) + (rd->READRATE * 0.25) > rd->READRATE)
        {
            printf("Level: %u\n", level);
            rd->data[rd->counter] = level ? '0' : '1';
            rd->values += level ? 0 : 1; // Add the level value to the values counter
            rd->counter++;
            rd->ptime = tick;
        }
    }

    if (rd->counter % BIT_COUNT == 0 && rd->counter > 0) //Every x values...
    {
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
        rd->data[MAX_BITS] = '\0'; // Ensure null termination
    }
    return;
}

struct ReadData* create_reader()
{
    struct ReadData *rd = malloc(sizeof(struct ReadData));
    rd->READRATE = 0;
    rd->ptime = 0;
    rd->tick1 = 0;
    rd->rateset = 0;
    rd->counter = 0;
    rd->values = 0;
    rd->run = 1;
    rd->data = malloc(MAX_BITS * sizeof(char) + 1);
    
    return rd;
}

void reset_reader(struct ReadData* rd)
{
    printf("Reset Reader...\n");
    rd->READRATE = 0;
    rd->ptime = 0;
    rd->tick1 = 0;
    rd->rateset = 0;
    rd->counter = 0;
    rd->values = 0;
    rd->run = 1;
    memset(rd->data, 0, sizeof(char) * (MAX_BITS + 1));
}

char* read_bits(struct ReadData* rd)
{
    while (rd->run)
    {
        fflush(stdout); // changed to a sleep to reduce CPU usage
    }
    time_sleep(.5); // This waits so that the resetting zero can be received.

    return rd->data;
}
