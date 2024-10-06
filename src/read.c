#include "read.h"

struct ReadData* rd = NULL; // Global variable to hold ReadData

void get_bit(int pi, unsigned gpio, unsigned level, uint32_t tick)
{
    printf("get_bit called: level=%u, tick=%u\n", level, tick);
    
    if (!(rd->run))
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
            int element = rd->counter / BIT_COUNT;
            int shift = rd->counter % BIT_COUNT;
            rd->data[element] |= ((level ? 0x00 : 0x01) << (BIT_COUNT - 1 - shift));
            rd->counter++;
            rd->ptime = tick;
            rd->last_bit_time = tick; // Update last_bit_time when a bit is received

            if (rd->counter == MAX_BYTES * BIT_COUNT)
            {
                rd->run = 0; // Stop reading to prevent buffer overflow
            }
        }
    }
    return;
}

struct ReadData* create_reader()
{
    rd = malloc(sizeof(struct ReadData));
    rd->READRATE = 0;
    rd->ptime = 0;
    rd->tick1 = 0;
    rd->rateset = 0;
    rd->counter = 0;
    rd->values = 0;
    rd->run = 1;
    rd->data = malloc(MAX_BYTES * sizeof(uint8_t));

    rd->last_bit_time = 0;

    // Calculate timeout_duration based on BAUDRATE
    double bit_time_us = 1e6 / BAUDRATE;
    rd->timeout_duration = (uint32_t)(3 * bit_time_us); // Timeout after 3 bit times

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
    rd->last_bit_time = 0;
    memset(rd->data, 0, sizeof(uint8_t) * MAX_BYTES);
}

uint8_t* read_bits(struct ReadData* rd)
{
    rd->last_bit_time = get_current_tick(1); 
    while (rd->run)
    {
        uint32_t current_time = get_current_tick(1);
        if ((current_time - rd->last_bit_time) > rd->timeout_duration)
        {
            rd->run = 0;
        }
    }
    printf("Data read\n");
    return rd->data;
}

struct Packet* generate_packet(uint8_t* data)
{
    struct Packet* newpack = malloc(sizeof(struct Packet));

    uint16_t temp = ((uint16_t)data[0] << 8) | data[1];
    newpack->dlength = (size_t)temp;

    printf("Data length extracted: %zu bytes\n", newpack->dlength);
    printf("Sending address: %02X\n", data[2]);
    printf("Receiving address: %02X\n", data[3]);

    if (newpack->dlength > MAX_BYTES - 4) {
        printf("Invalid data length\n");
        free(newpack);
        return NULL;
    }

    newpack->sending_addy = data[2];
    newpack->receiving_addy = data[3];
    newpack->data = malloc(newpack->dlength);

    memcpy(newpack->data, &data[4], newpack->dlength);

    printf("Packet data:\n");
    for (size_t i = 0; i < newpack->dlength; i++) {
        printf("%02X ", newpack->data[i]);
    }
    printf("\n");

    return newpack;
}

