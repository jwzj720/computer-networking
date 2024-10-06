#include "read.h"

void get_bit(int pi, unsigned gpio, unsigned level, uint32_t tick, void* user)
{
    struct ReadData* rd = (struct ReadData*) user;

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
    struct ReadData *rd = malloc(sizeof(struct ReadData));
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
    rd->last_bit_time = gpioTick(); // Initialize last_bit_time
    while (rd->run)
    {
        uint32_t current_time = gpioTick();
        if ((current_time - rd->last_bit_time) > rd->timeout_duration)
        {
            rd->run = 0; // Stop reading due to timeout
        }
        time_sleep(0.001); // Sleep to reduce CPU usage
    }
    printf("Data read\n");
    return rd->data;
}

struct Packet* generate_packet(uint8_t* data)
{
    struct Packet* newpack = malloc(sizeof(struct Packet));

    uint16_t temp = ((uint16_t)data[0] << 8) | data[1];
    newpack->dlength = (size_t)temp;
    newpack->sending_addy = data[2];
    newpack->receiving_addy = data[3];
    newpack->data = (uint8_t *)malloc(sizeof(uint8_t) * newpack->dlength);

    memcpy(newpack->data, &data[4], newpack->dlength);

    return newpack;
}
