#ifndef READ_H
#define READ_H

#include <pigpiod_if2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "build_packet.h"
#define BIT_COUNT 8
#define MAX_BYTES 50

/*
 * ReadData struct is implemented at link layer. Used to read correct voltage changes 
 * in order to decode bits.
 */
struct ReadData{
    int id; /* the ID of the user from which data is being read */
    uint32_t READRATE;
    uint32_t ptime;
    uint32_t tick1;
    int rateset;
    int counter;
    int values;
    int run;
    uint8_t* data;
};

/*
 * Packet structure. Requires dlength, sending_addy and receiving_addy to be a valid packet.
*/
struct Packet{
    size_t dlength; // Number of bytes in data
    uint8_t sending_addy; 
    uint8_t receiving_addy;
    uint8_t* data;
};

struct ReadData* create_reader(int this_id);
void reset_reader(struct ReadData* rd);
void get_bit(int pi, unsigned gpio, unsigned level, uint32_t tick, void* user);
uint8_t* read_bits(struct ReadData* rd); /* An example function declaration */
struct Packet* generate_packet(uint8_t* data);

#endif
