#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define MAX_INPUT_LENGTH 280
#define DATA_BLOCK_SIZE 4
#define CODEWORD_SIZE 8

// -------------- ASCII TEXT ENCODING AND DECODING --------------

/* Application to take in text and return it as a uint8_t array of hex values
*/
uint8_t* text_to_bytes(size_t* len){
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

    for (size_t i = 0; i < *len; i++) {
        printf("%02X ", hexList[i]); // Print each byte in hex format
    }
    printf("<- Hex values");

    printf("\n");

    return hexList; // return the byte array
}

/* Application to turn hex vals back to ASCII text
*/
char* bytes_to_text(const uint8_t* bytes, size_t len){
    char* text_out = malloc(len+1);

    for (size_t i = 0; i < len; i++) {
        text_out[i] = (char)bytes[i];  // convert each byte to a char
    }

    text_out[len] = '\0'; 

    return text_out;
}

// -------------- HAMMING 8,4 ENCODING AND DECODING --------------
// based on (7,4) algorithm outlined here: https://www.geeksforgeeks.org/hamming-code-in-computer-network/ 

// Bit Position:  8  7  6  5  4  3  2  1
// Bit Role:     p8 d3 p4 d2 p2 d1 p1 d0

/* Hamming encodes a nibble of data from OG payload

INPUT:
uint8_t nibble: nibble of data from the payload

OUTPUT:
uint8_t ham_nibble: byte of hamming encoded data
*/    
uint8_t nibble_encode(uint8_t nibble){
    // inst data bits
    uint8_t d3 = (nibble >> 3) & 1;
    uint8_t d2 = (nibble >> 2) & 1;
    uint8_t d1 = (nibble >> 1) & 1;
    uint8_t d0 = nibble & 1;

    // Calculate parity bits based solely on data bits
    uint8_t p1 = d0 ^ d1 ^ d2 ^ d3;    // Parity for bits 1,3,5,7
    uint8_t p2 = d1 ^ d2 ^ d3;          // Parity for bits 2,3,6,7
    uint8_t p4 = d2 ^ d3;               // Parity for bits 4,5,6,7
    uint8_t p8 = p1 ^ p2 ^ p4 ^ d0 ^ d1 ^ d2 ^ d3; // Overall parity

    return (p8 << 7) | (d3 << 6) | (p4 << 5) | (d2 << 4) | (p2 << 3) | (d1 << 2) | (p1 << 1) | d0;
}
/* Hamming encodes byte array in accordance with hamming 8,4 (this doubles payload length)

INPUTS:
uint8_t* payload: pointer to the byte array of payload vals
size_t len: length of payload
encoded_len: pointer address to store the length of encoded payload

OUTPUT:
uint8_t* encoded: address to byte array containing encoded payload
*/
uint8_t* ham_encode(uint8_t* payload, size_t len, size_t* encoded_len)
{ 
    *encoded_len = len * 2; // because we are using 8,4 each byte is double because split in half
    uint8_t* encoded = malloc(*encoded_len * sizeof(uint8_t));

    for (size_t i = 0; i < len; i++) {
        // Split each byte into two 4-bit nibbles
        uint8_t high_nibble = (payload[i] >> 4) & 0x0F; // upper 4 bits
        uint8_t low_nibble = payload[i] & 0x0F; // lower 4 bits

        // encode each nibble
        encoded[i * 2] = nibble_encode(high_nibble);
        encoded[i * 2 + 1] = nibble_encode(low_nibble);
    }

    return encoded;
}

/* Individual byte decode  —  and correction if needed

INPUT:
uint8_t encoded: hamming encoded byte that has been through a noise ridden environments

OUTPUT:
uint8_t cooked_ham: corrected nibble of original length
*/
uint8_t nibble_decode(uint8_t encoded) {

    // extract bits
    uint8_t p8 = (encoded >> 7) & 1;
    uint8_t d3 = (encoded >> 6) & 1;
    uint8_t p4 = (encoded >> 5) & 1;
    uint8_t d2 = (encoded >> 4) & 1;
    uint8_t p2 = (encoded >> 3) & 1;
    uint8_t d1 = (encoded >> 2) & 1;
    uint8_t p1 = (encoded >> 1) & 1;
    uint8_t d0 = encoded & 1;

    // Recalculate parity bits based on data bits
    uint8_t expected_p1 = d0 ^ d1 ^ d2 ^ d3;
    uint8_t expected_p2 = d1 ^ d2 ^ d3;
    uint8_t expected_p4 = d2 ^ d3;
    uint8_t expected_p8 = p1 ^ p2 ^ p4 ^ d0 ^ d1 ^ d2 ^ d3;
    
    // calculate syndrome bits
    uint8_t s1 = p1 ^ expected_p1;
    uint8_t s2 = p2 ^ expected_p2;
    uint8_t s4 = p4 ^ expected_p4;
    uint8_t s8 = p8 ^ expected_p8;

    // combine syndrome bits into a single value
    uint8_t syndrome = (s8 << 3) | (s4 << 2) | (s2 << 1) | s1;

    if (syndrome != 0) {
        printf("Error detected! Syndrome: %d\n", syndrome);
        if (syndrome <= 8) { // Valid bit position
            // Correct the error by flipping the bit at the syndrome position
            encoded ^= (1 << (syndrome - 1));

            // Re-extract corrected bits
            p8 = (encoded >> 7) & 1;
            d3 = (encoded >> 6) & 1;
            p4 = (encoded >> 5) & 1;
            d2 = (encoded >> 4) & 1;
            p2 = (encoded >> 3) & 1;
            d1 = (encoded >> 2) & 1;
            p1 = (encoded >> 1) & 1;
            d0 = encoded & 1;

            // Optionally, you can recalculate and verify parity bits again
        } else {
            printf("Syndrome points to an invalid bit position.\n");
        }
    }
    // extract the corrected data bits
    uint8_t decoded = (d3 << 3) | (d2 << 2) | (d1 << 1) | d0;

    return decoded;
}

/* Decode hamming 8,4 and correct single bit errors in byte array generated from hamload()

INPUT:
uint8_t* hamload: address to encoded byte array
size_t encoded_len: length of encoded payload
size_t* decoded_len: length of decoded/OG payload

OUTPUT:
uint8_t* decoded: original payload pre-transmission (unless too many errors created in transmission...)
*/
uint8_t* ham_decode(uint8_t* encoded_array, size_t encoded_len, size_t* decoded_len){
    *decoded_len = encoded_len / 2;  // each pair decodes to one byte
    uint8_t* decoded_array = malloc(*decoded_len * sizeof(uint8_t));

    for (size_t i = 0; i < *decoded_len; i++) {
        // Decode two 4-bit nibbles from the encoded data
        uint8_t high_nibble = nibble_decode(encoded_array[i * 2]);
        uint8_t low_nibble = nibble_decode(encoded_array[i * 2 + 1]);

        // Combine the two nibbles into one byte
        decoded_array[i] = (high_nibble << 4) | low_nibble;
    }
return decoded_array;
}

int main(){
    
    // gen bytes
    size_t data_size;
    uint8_t* payload = text_to_bytes(&data_size);

    // encode bytes
    size_t encoded_len;
    uint8_t* encoded_array = ham_encode(payload, data_size, &encoded_len);

    // // print the encoded byte array
    // printf("Encoded Hamming (8,4) values: \n");
    // for (size_t i = 0; i < encoded_len; i++) {
    //     printf("%02X ", encoded_array[i]);
    // }
    // printf("\n");

    // decode (WHAT DO WE DO WITH ENCODED LEN???)
    size_t decoded_len;
    uint8_t* decoded_array = ham_decode(encoded_array, encoded_len, &decoded_len);

    // print the decoded byte array
    for (size_t i = 0; i < decoded_len; i++) {
        printf("%02X ", decoded_array[i]);
    }
    printf("<- Decoded Hamming (8,4) values \n");

    // hex to ascii
    char* end_text = bytes_to_text(decoded_array, decoded_len);
    printf("Received message: %s\n", end_text);


    free(payload);
    free(encoded_array);
    free(decoded_array);
    free(end_text);
    return 0;
}