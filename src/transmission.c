// collection of functions to send over and read from the line

#include "transmission.h"
#include "build_packet.h"
#include "encoding.h"

int send_to_file(int pi)
{
    uint8_t device_addr =   0x01;  // Single 8-bit device address
    uint8_t receiver_addr = 0x09;  // Single 8-bit receiver address

    size_t payload_length;
    uint8_t* payload = text_to_bytes(&payload_length);

    size_t encoded_length;
    uint8_t* hamload = ham_encode(payload, payload_length, &encoded_length);

    printf("Size of encoded packet %ld\n", encoded_length);

    uint8_t packet[50];
    size_t data_size = sizeof(payload);
    int packet_size = build_packet(device_addr, receiver_addr, hamload, encoded_length, packet);

    if (send_bytes(packet, packet_size, GPIO_SEND, pi) != 0)
    {
        fprintf(stderr, "Failed to send packet\n");
        return 1;
    }

    return 0;
}
