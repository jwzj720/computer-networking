#ifndef PACKET_DEFS_H
#define PACKET_DEFS_H

#define MAX_PACKET_SIZE 1024  // Adjust as needed
#define MAX_DATA_SIZE (MAX_PACKET_SIZE - 4)  // Subtract header size
#define ADDRESS_SIZE_BYTES 1 
#define DATA_LENGTH_SIZE_BYTES 2 
#define BIT_COUNT 8
#define MAX_BYTES 1024


/*
 * Packet structure. Requires dlength, sending_addy and receiving_addy to be a valid packet.
*/
struct Packet{
    size_t dlength; // Number of bytes in data
    uint8_t sending_addy; 
    uint8_t receiving_addy;
    uint8_t* data;
};

#endif /* PACKET_DEFS_H */