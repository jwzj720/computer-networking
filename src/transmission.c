// collection of functions to send over and read from the line

#include "transmission.h"
#include "build_packet.h"
int send_to_file(int pi)
{
    // collect user input
    //char input[MAX_INPUT_LENGTH + 1];
    //printf("Enter text to send: ");
    //fgets(input,sizeof(input),stdin);

    // remove newline and insert null
    //size_t input_len = strlen(input);
    //if (input[input_len - 1]=='\n'){
    //    input[input_len - 1] = '\0';
    //}


    // encode ASCII to binary, add parity bits according to hamming (7,4), add leading and trailing bits
    //char* binary = text_to_binary(input);
    //free(input);
    uint8_t device_addr =   0x01;  // Single 8-bit device address
    uint8_t receiver_addr = 0x09;  // Single 8-bit receiver address
    uint8_t data[4] =       {0xAA, 0xCC, 0xF0, 0x0F};  // 4 bytes of data

    uint8_t packet[50];
    size_t data_size = sizeof(data);

    int packet_size = build_packet(device_addr, receiver_addr, data, data_size, packet);
    //printf("text_to_binary result (COMPARE THIS) %s\n", binary);

    //char* hamming_binary = hamming_encode_full(binary);
    
    //free(binary);
    //char* packed = pack(hamming_binary);
    //free(hamming_binary);


   // send data over the line (and don't worry if one bit is flipped in transmission!)
    if (send_bytes(packet, packet_size, GPIO_SEND, pi)!=0)
    {
	printf("Bit send error.\n");
        return 1;
    }
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
