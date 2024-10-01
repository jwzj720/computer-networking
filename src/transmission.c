// collection of functions to send over and read from the line

#include "transmission.h"

int send_to_file()
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
    char* hamming_binary = hamming_encode_full(binary);

    // printing validation
    printf("Binary %s\n", binary);
    printf("Binary %s\n", hamming_binary);

    // send data over the line (and don't worry if one bit is flipped in transmission!)
    send_bits(hamming_binary);
    return 0;
}

int read_to_file(struct ReadData* rd)
{
    // read bits from line and store as char*
    char* result = read_bits(rd);
    printf("Binary received: %s\n", result);

    // do error detection/correction and remove redundant data created in hamming encoding (this return should be same binary as text_to_binary return)
    char* hamming_decode = hamming_decode_full(result);

    // convert binary back to ASCII
    char* final = binary_to_text7(hamming_decode);

    // print final message
    printf( "Results: %s\n", final);
    return 0;
}
