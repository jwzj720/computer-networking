// collection of functions to encode and decode (text to binary) and (hamming (7,4)) and to do 1 bit error correction

#include "encoding.h"

// -------------- ASCII TEXT ENCODING AND DECODING --------------

/*
Application to take in text and return it as a uint8_t array of hex values
*/
uint8_t text_to_bytes(size_t* len){
    // collect user input
    char input[MAX_INPUT_LENGTH + 1];
    printf("Please enter a message: ");
    fgets(input,sizeof(input),stdin);

    *len = strlen(input); // get length of input
    if (input[*len-1] == '\n') input[*len-1] = '\0';  // Remove newline character
    *len = strlen(input);

    // Allocate memory for the byte list (uint8_t array)
    uint8_t* hexList = malloc(*len * sizeof(uint8_t));

    // convert each char to byte and add to the list
    for (size_t i = 0; i < *len; i++) {
        hexList[i] = (uint8_t)input[i]; // Store ASCII as raw bytes
    }

    printf("Hex values: ");
    for (size_t i = 0; i < *len; i++) {
        printf("%02X ", hexList[i]); // Print each byte in hex format
    }

    printf("\n");

    return hexList; // return the byte array
}

/*
Application to turn hex vals back to ASCII text
*/
uint8_t bytes_to_text(uint8_t* bytes, size_t len){
    char* text = (char*)malloc(len+1);

    for (size_t i = 0; i < len; i++) {
        text[i] = (char)bytes[i];  // convert each byte to a char
    }

    text[len] = '\0'; 

    return text;
}

// -------------- HAMMING 8,4 ENCODING AND DECODING --------------

// based on (7,4) algorithm outlined here: https://www.geeksforgeeks.org/hamming-code-in-computer-network/ 

// encoding inserts redundant bits in inputted string and then returns for transmission
// decoding checks for corruption, repairs if so, and then returns output sans redundant bits

// this function calculates and inserts parity bits for a single 4-bit data block in accordince with hamming 8,4 (this doubles payload length)

// INPUTS:
    // nibble: a half byte of data to be returned as two bytes representing a nibble of og data each

uint8_t hamload(uint8_t payload)
{
    // setup
    size_t len = strlen(payload);
    size_t blocks = (len + DATA_BLOCK_SIZE - 1) / DATA_BLOCK_SIZE; // ceiling division so padding is added to cases where length is not divisible by 4
    size_t encoded_len = blocks * CODEWORD_SIZE;
    uint8_t encoded = malloc(encoded_len + 1);


    // iterate through payload, select item, divide it, encode it


}

// function to encode entire binary string with hamming (7,4)
uint8_t hamming_encode(uint8_t pay_nibble)
{
    
    //encoded_string[0] = '\0'; // Initialize as empty string

    for (size_t i = 0; i < blocks; i++) {
        char data_block[DATA_BLOCK_SIZE + 1] = {'0', '0', '0', '0', '\0'}; // Initialize with '0's

        // Copy up to 4 bits from binary_string to data_block
        for (size_t j = 0; j < DATA_BLOCK_SIZE; j++){
            size_t bit_index = i * DATA_BLOCK_SIZE + j;
            if (bit_index < length){
                char bit = binary_string[bit_index];
                data_block[j] = bit;
        }
    }
    char* codeword = hamming_encode(data_block);

    if (codeword == NULL) {
        free(encoded_string);
        return NULL;
        }
        // Append the 7-bit codeword to the encoded string
        strcat(encoded_string, codeword);
        free(codeword);
}

encoded_string[encoded_length] = '\0'; // Null-terminate the final encoded string
return encoded_string;
}














// function to decode individual hamming (7,4) binary strings
char* hamming_decode(char* codeword)
{
    if (strlen(codeword) != CODEWORD_SIZE) {
        fprintf(stderr, "Error: Codeword must be exactly 7 bits.\n");
        return NULL;
    }

    // convert codeword to bits
    int bits[7];
    for (int i = 0; i < 7; i++) {
        bits[i] = codeword[i] - '0';
    }

    // Calculate syndrome bits (these indicate if there is an error)
    int S1 = bits[0] ^ bits[2] ^ bits[4] ^ bits[6];
    int S2 = bits[1] ^ bits[2] ^ bits[5] ^ bits[6];
    int S3 = bits[3] ^ bits[4] ^ bits[5] ^ bits[6];

    // Calculate syndrome value
    int syndrome = (S3 << 2) | (S2 << 1) | S1;

    // If syndrome is non-zero, there is an error in the bit at position 'syndrome'
    if (syndrome != 0) {
        if (syndrome >= 1 && syndrome <= 7) {
            printf("Error detected at bit position %d. Correcting...\n", syndrome);
            bits[syndrome - 1] ^= 1; // Correct the bit
        } else {
            printf("Syndrome value out of range. Unable to correct.\n");
        }
    }

    // Extract data bits: D1 (bits[2]), D2 (bits[4]), D3 (bits[5]), D4 (bits[6])
    char* data_bits = malloc(DATA_BLOCK_SIZE + 1);

    data_bits[0] = bits[2] + '0';
    data_bits[1] = bits[4] + '0';
    data_bits[2] = bits[5] + '0';
    data_bits[3] = bits[6] + '0';
    data_bits[4] = '\0';

    return data_bits;
}

// function to decode entire hamming strings of arbitrary length
char* hamming_decode_full(char* encoded_string)
{
    size_t length = strlen(encoded_string);

    if (length % CODEWORD_SIZE != 0) {
        fprintf(stderr, "Error: Encoded string length must be a multiple of 7.\n");
        return NULL;
    }

    size_t blocks = length / CODEWORD_SIZE;
    size_t decoded_length = blocks * DATA_BLOCK_SIZE;
    char* decoded_string = malloc(decoded_length + 1); // +1 for null terminator

    decoded_string[0] = '\0'; // Initialize as empty string

    for (size_t i = 0; i < blocks; i++) {
        char codeword[CODEWORD_SIZE + 1];
        strncpy(codeword, &encoded_string[i * CODEWORD_SIZE], CODEWORD_SIZE);
        codeword[CODEWORD_SIZE] = '\0';

        char* data_bits = hamming_decode(codeword);
        if (data_bits == NULL) {
            free(decoded_string);
            return NULL;
        }

        strcat(decoded_string, data_bits);
        free(data_bits);
    }

    decoded_string[decoded_length] = '\0'; // Null-terminate the decoded string
    return decoded_string;
}

// -------------- PACKING --------------

// add '10' as leading bits, add '11111110' as tailing bits
char* pack(char* pack_me)
{
    size_t send_len = strlen(pack_me) + 10; // +2 for '10' +7 for '1111111' +1 for '0'
    char* send_bin = malloc(send_len + 1); // +1 for '\0'

    // add '10' to begin sequence
    send_bin[0] = '1';
    send_bin[1] = '0';

    // fill send_bin with hamming_binary data
    for (size_t i = 2; i < send_len - 8; i++){
           send_bin[i] = pack_me[i-2];
    }

    // make the 7 characters 2 from the end of send_bin '1'
    for (size_t i = send_len - 8; i< send_len; i++){
           send_bin[i]= '1';
    }

    send_bin[send_len -1] ='0'; // add final level bit
    send_bin[send_len] = '\0'; // add null terminator bit

    return send_bin;
}

// remove tailing '1111111'
char* unpack(char* unpack_me)
{
    size_t len = strlen(unpack_me) - 7;
    char* no_ones = malloc(len + 1);

    // fill no_ones with unpack_me data
    for (size_t i = 0; i < len ; i++){
           no_ones[i] = unpack_me[i];
    }

    printf("ones removed: %s\n", no_ones);

    return no_ones;
}
