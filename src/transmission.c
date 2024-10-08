// collection of functions to send over and read from the line

#include "transmission.h"
#include "build_packet.h"

int send_to_file(int pi)
{
    uint8_t device_addr =   0x01;  // Single 8-bit device address
    uint8_t receiver_addr = 0x09;  // Single 8-bit receiver address

    size_t data_size;
    uint8_t* payload = text_to_bytes(&data_size);
    // uint8_t hamload =
    //
    printf("Size of packet %ld\n", data_size);
    uint8_t packet[50];

    int packet_size = build_packet(device_addr, receiver_addr, payload, data_size, packet);
    printf("packet size: %d\n", packet_size);

    if (send_bytes(packet, packet_size, GPIO_SEND, pi) != 0)
    {
        fprintf(stderr, "Failed to send packet\n");
        return 1;
    }

    return 0;
}
