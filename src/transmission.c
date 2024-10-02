// collection of functions to send over and read from the line

#include "transmission.h"

int send_to_file(int pi)
int send_to_file(int pi)
{
    // collect user input
    char input[MAX_INPUT_LENGTH + 1];
    printf("Enter text to send: ");
    fgets(input,sizeof(input),stdin);

    // remove newline and insert null
    size_t input_len = strlen(input);
    if (input[input_len - 1]=='\n'){
        input[input_len - 1] = '\0';
    }

    // encode ASCII to binary and add parity bits according to hamming (7,4)
    char* binary = text_to_binary(input);
    //char* hamming_binary = hamming_encode_full(binary);

    // printing validation
    //printf("Binary %s\n", hamming_binary);
    //printf("Binary %s\n", hamming_binary);
    printf("Length of og: %ld\n", strlen(binary));
    size_t send_len = strlen(hamming_binary) + 10;
    printf("Length of new: %ld\n", send_len);
    char* send_bin = malloc(send_len +1);

    send_bin[0] = '1';
    send_bin[1] = '0';

    for (size_t i = 2; i < send_len - 8; i++){
	    send_bin[i] = hamming_binary[i-2];
    }

    for (size_t i = send_len -8; i< send_len; i++){
	    send_bin[i]= '1';
    }

    send_bin[send_len] ='0';// add final level bit
    printf("Data Being Sent: %s\n", send_bin);

    // send data over the line (and don't worry if one bit is flipped in transmission!)
    if (send_bits(send_bin, GPIO_SEND, pi)!=0)
    {
	printf("Bit send error.\n");
        return 1;
    }
    return 0;
}

int read_to_file(struct ReadData* rd)
int read_to_file(struct ReadData* rd)
{
    // read bits from line and store as char*
    char* result = read_bits(rd);
    //printf("Binary received: %s\n", result);

    // do error detection/correction and remove redundant data created in hamming encoding (this return should be same binary as text_to_binary return)
    char* hamming_decode = hamming_decode_full(result);
    printf("Decoded Binary: %s\n", hamming_decode);

    // convert binary back to ASCII
    char* final = binary_to_text(hamming_decode);

    // print final message
    printf( "Results: %s\n", final);
    return 0;
}
