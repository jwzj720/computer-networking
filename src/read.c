#include "read.h"


void get_bit(int pi, unsigned gpio, unsigned level, uint32_t tick, void* user) // added userdata for struct
{
    struct ReadData* rd = (struct ReadData*) user;
  //  printf("READRATE: %"PRIu32" ; ",rd->READRATE);
   // printf("Tick: %"PRIu32" ; ",tick);
    if(!(rd->run))
    
    {
//	printf("Bit should not read");    
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
        //printf("timegap: %"PRIu32" ; ",tick-ptime);
        // if the difference since the last tick is significantly less than expected readtime,
        // it is probably one that we want to ignore.
        if ((tick - rd->ptime) + (rd->READRATE * 0.25) > rd->READRATE)
        {
  //          printf("Level: %u ;\n", level);
            rd->data[rd->counter] = level ? '0' : '1';
            rd->values += level ? 0 : 1; // Add the level value to the values counter
            rd->counter++;
            rd->ptime = tick;
        }
    }

    if (rd->counter % BIT_COUNT == 0 && rd->counter > 0) //Every x values...
    {
    //    printf("Values: %d ; ", rd->values);
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
//	printf("Resetting values...\n");
    rd->READRATE = 0;
    rd->ptime = 0;
    rd->tick1 = 0;
    rd->rateset = 0;
    rd->counter = 0;
    rd->values = 0;
    rd->run = 1;
    memset(rd->data, 0, sizeof(char) * (MAX_BITS + 1));
}


char* read_message(struct ReadData* rd)
{
    while (rd->run)
    {
        fflush(stdout); // changed to a sleep to reduce CPU usage
    }
    time_sleep(.5);
    return rd->data;
}


/*
* General message reader. This should always be running, and repeatedly call read_packet
*/
char* read_packet(struct ReadData* rd)
{
    // Read Byte by Byte. Stop sequence will eventually be removed
    char currbyte[7] = read_byte(rd);
    //Set a max_counter according to data_length described in packet header.
    while (rd->run)
    {
        fflush(stdout); // changed to a sleep to reduce CPU usage
    }
    time_sleep(.5);
    return rd->data;
}

char* read_byte(struct ReadData* rd)
{
    while (rd->run)
    {
        fflush(stdout); // changed to a sleep to reduce CPU usage
    }
}
