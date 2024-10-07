// collection of functions to send over and read from the line

#include "transmission.h"
#include "build_packet.h"

int send_to_file(int pi)
{
    uint8_t device_addr =   0x01;  // Single 8-bit device address
    uint8_t receiver_addr = 0x09;  // Single 8-bit receiver address

    uint8_t payload = text_to_bytes();
    // uint8_t hamload = 
    uint8_t packet[50];
    size_t data_size = sizeof(payload);

    int packet_size = build_packet(device_addr, receiver_addr, payload, data_size, packet);

    return 0;
}

int read_to_file(struct ReadData* rd)
{
    // read bits from line and store as char*
    char* result =(char*) read_bits(rd);

    // '10' should be removed here along with the trailing '0'
    printf("Binary First Received: %s\n", result);

    // remove '1111111' from the tail
    char* cleaned = unpack(result);
    //free(result);

    // do error detection/correction and remove redundant data created in hamming encoding
    char* hamming_decode = hamming_decode_full(cleaned);
    free(cleaned);

    // this should print the same result as the text_to_binary print on sending computer
    printf("Hamming Decoded Binary: %s\n", hamming_decode);

    // convert binary back to ASCII
    char* final = binary_to_text(hamming_decode);
    free(hamming_decode);

    // print final message
    printf( "Results: %s\n", final);
    free(final);
    return 0;
}
