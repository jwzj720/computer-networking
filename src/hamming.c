// encoding inserts redundant bits in inputted string and then returns for transmission
// decoding checks for corruption, repairs if so, and then returns output sans redundant bits

// based on algorithm outlined here: https://www.geeksforgeeks.org/hamming-code-in-computer-network/ 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "hamming.h"

// this function calculates and inserts parity bits for a single 4-bit data block in accordince with hamming 7,4
char* hamming_encode(char* data_block)
{ 
    if (strlen(data_block) != DATA_BLOCK_SIZE) {
        fprintf(stderr, "Data block must be exactly 4 bits.\n");
        return NULL;
    }
    char* codeword = malloc(CODEWORD_SIZE + 1); // 7-bit codeword + null terminator

    // Assign position bits
    // Position: 1 2 3 4 5 6 7
    // Bits:     P1 P2 D1 P3 D2 D3 D4
    codeword[2] = data_block[0];
    codeword[4] = data_block[1];
    codeword[5] = data_block[2];
    codeword[6] = data_block[3];

    // Calculate parity bits
    // P1 covers bits 1,3,5,7 (P1, D1, D2, D4)
    int P1 = (data_block[0] - '0') ^ (data_block[1] - '0') ^ (data_block[3] - '0');
    // P2 covers bits 2,3,6,7 (P2, D1, D3, D4)
    int P2 = (data_block[0] - '0') ^ (data_block[2] - '0') ^ (data_block[3] - '0');
    // P3 covers bits 4,5,6,7 (P3, D2, D3, D4)
    int P3 = (data_block[1] - '0') ^ (data_block[2] - '0') ^ (data_block[3] - '0');

    codeword[0] = P1 + '0'; // P1
    codeword[1] = P2 + '0'; // P2
    codeword[3] = P3 + '0'; // P3

    codeword[7] = '\0'; // Null terminator for string

    return codeword;
    }


// function to encode entire binary string with hamming (7,4)
char* hamming_encode_full(char* binary_string)
{
    size_t length = strlen(binary_string);
    size_t blocks = (length + DATA_BLOCK_SIZE - 1) / DATA_BLOCK_SIZE; // ceiling division so padding is added to cases where length is not divisible by 4
    size_t encoded_length = blocks * CODEWORD_SIZE;
    char* encoded_string = malloc(encoded_length + 1); 

    encoded_string[0] = '\0'; // Initialize as empty string

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
char* hamming_decode(char* codeword){
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
char* hamming_decode_full(char* encoded_string){
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

// int main() {
//     // Test cases

//     // TAKEAWAYS:
//         // Currently if input is not of a length divisible by 7, decoding will not work
//             // Need to add some sort of padding

//     char* test_inputs[] = {
//         "1011",       // Length 4 (exact)
//         "10110011",   // Length 8 (exact multiple of 4)
//         "101",        // Length 3 (needs padding)
//         "1011001",    // Length 7 (needs padding)
//         "",           // Empty string
//         "110010101011" // Length 12 (exact multiple of 4)
//         "1110001010101010100100010101010100011111001001010101010101001101010"
//     };

//     size_t num_tests = sizeof(test_inputs) / sizeof(test_inputs[0]);

//     for (size_t i = 0; i < num_tests; i++) {
//         printf("Test Input %zu: %s\n", i + 1, test_inputs[i]);
//         char* encoded = hamming_encode_full(test_inputs[i]);
//         char* decoded = hamming_decode_full(test_inputs[i]);
//         if (encoded != NULL) {
//             printf("Encoded Hamming (7,4) binary: %s\n", encoded);
//             printf("Decoded Hamming (7,4) binary: %s\n", decoded);
//             free(encoded);
//             free(decoded);
//         } else {
//             printf("Encoding failed for Test Input %zu.\n\n", i + 1);
//         }
//     }
//     return 0;
// }


