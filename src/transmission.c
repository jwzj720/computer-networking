// collection of functions to send over and read from the line

#include "transmission.h"
#include "build_packet.h"
int send_to_file(int pi)
{
    
    uint8_t device_addr =   0x01;  // Single 8-bit device address
    uint8_t receiver_addr = 0x09;  // Single 8-bit receiver address
    uint8_t data[4] =       {0xAA, 0xCC, 0xF0, 0x0F};  // 4 bytes of data

    uint8_t packet[50];
    size_t data_size = sizeof(data);
    int packet_size = build_packet(device_addr, receiver_addr, data, data_size, packet);

    return 0;
}
