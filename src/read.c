#include "read.h"


void get_bit(int pi, unsigned gpio, unsigned level, uint32_t tick, void* user) // added userdata for struct
{
    struct ReadData* rd = (struct ReadData*) user;
    printf("READRATE: %"PRIu32" ; ",rd->READRATE);
    printf("Tick: %"PRIu32" ; ",tick);
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
            printf("Level: %u ;\n", level);
            // Counter doesn't change, should get data counter/8.

            // Get the current int to be looking at, as well as the bit position
            int element = rd->counter/BIT_COUNT;
            int shift = rd->counter % BIT_COUNT;
            // Bit shift expected value to appropriate location
            rd->data[element] |= ((level ? 0x00 : 0x01) << (BIT_COUNT-1-shift));
	    printf("pos: %d ;",shift);
  	    printf("element: %"PRIu8"\n",rd->data[element]);
             // Add the level value to the values counter
            rd->counter++;
            rd->ptime = tick;
	    
            if ((rd->data[element] == 0xFF) || (rd->counter == MAX_BYTES*BIT_COUNT))
            {
            	printf("end sequence read");
		rd->run=0;
            }
        }
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
    rd->data = malloc(MAX_BYTES * sizeof(uint8_t));
    
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
    memset(rd->data, 0, sizeof(uint8_t) * MAX_BYTES);
}


uint8_t* read_bits(struct ReadData* rd)
{
    while (rd->run)
    {
        fflush(stdout); // changed to a sleep to reduce CPU usage
    }
    time_sleep(.5);

    //Parse out stop sequence
    printf("Data read\n");
    uint8_t* data_copy = malloc(MAX_BYTES * sizeof(uint8_t));
    memcpy(data_copy, rd->data, MAX_BYTES * sizeof(uint8_t));    
    return data_copy;
}


/*
* Take read data and convert it to packets
*/
struct Packet* generate_packet(uint8_t* data)
{
    struct Packet* newpack = malloc(sizeof(struct Packet));
    // TODO: Handle bad packet headers (Right now not having enough received data will cause
    // a seg fault due to ArrayOutOfBounds)
    uint16_t temp = ((uint16_t)data[0] << 8) | data[1];
    newpack->dlength = (size_t)temp;
    newpack->sending_addy = data[2];
    newpack->receiving_addy = data[3];
    newpack->data = (uint8_t *)malloc(sizeof(uint8_t) * newpack->dlength); //This multiplies by uint16_t, potential undefined behavior?

    //Put the remaining data into the newpack->data spot.
    memcpy(newpack->data, &data[4],newpack->dlength);
//    print_packet_binary((uint8_t*)newpack);
    //Packet has been created, now return

    return newpack;
}

