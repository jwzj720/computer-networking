#include "read.h"
void get_bit(int pi, unsigned gpio, unsigned level, uint32_t tick, void* user) 
{
    struct ReadData* rd = (struct ReadData*) user;
    if(!(rd->run))
    {
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
        if ((tick - rd->ptime) + (rd->READRATE * 0.25) > rd->READRATE)
        {
            int element = rd->counter/BIT_COUNT;
            int shift = rd->counter % BIT_COUNT;
            rd->data[element] |= ((level ? 0x00 : 0x01) << (BIT_COUNT-1-shift));
            rd->counter++;
            rd->ptime = tick;
	    
            if ((rd->data[element] == 0xFF) || (rd->counter == MAX_BYTES*BIT_COUNT))
            {
                rd->run=0;
            }
        }
    }
    return;
}

struct ReadData* create_reader(int this_id)
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
        fflush(stdout); 
    }
    time_sleep(.5);
    //Parse out stop sequence
    printf("Data read\n");
    return rd->data;
}


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
    //Packet has been created, now return
    return newpack;
}
