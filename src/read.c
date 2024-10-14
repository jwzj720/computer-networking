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
        printf("READRATE: %u microseconds, ptime set to: %u.\n", rd->READRATE, rd->ptime);

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
            
            printf("Bit captured: %d (shift: %d, element: %d), Data: 0x%X\n", level ? 0 : 1, shift, element, rd->data[element]);

	    
            if ((rd->data[element] == 0xFF) || (rd->counter == MAX_BYTES*BIT_COUNT))
            {
                rd->run=0;
                printf("Terminating bit reading, either full 0xFF received or max bytes reached.\n");
            }
            
        }
        printf("Tick rate mismatch, skipping bit capture. Time difference: %u, Expected READRATE: %u\n", tick - rd->ptime, rd->READRATE);
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
    rd->id = this_id;   
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
    printf("Data read complete. Data captured:\n");
    for (int i = 0; i < MAX_BYTES; i++) {
        printf("0x%02X ", rd->data[i]);
    }
    printf("\n");
    return rd->data;
}


/*
* Take read data and convert it to packets
* Right now this funciton re-allocates memory every packet receipt.
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
    //Packet has been created, now return
    printf("Packet generated: Length: %zu, Sending Address: 0x%02X, Receiving Address: 0x%02X\n", 
           newpack->dlength, newpack->sending_addy, newpack->receiving_addy);
    printf("Packet data:\n");
    for (int i = 0; i < newpack->dlength; i++) {
        printf("0x%02X ", newpack->data[i]);
    }
    printf("\n");

    return newpack;
}
