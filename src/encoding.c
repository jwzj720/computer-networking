// collection of functions to encode and decode (text to binary) and (hamming (7,4)) and to do 1 bit error correction

#include "encoding.h"

// -------------- ASCII TEXT ENCODING AND DECODING --------------

char* text_to_binary(const char* text) {
    size_t len = strlen(text);
    // 2 for "10" + 7 bits per char + 7 for "0000000"
    //size_t binary_len = len * 7 + 7;
    size_t binary_len = len * 7;
    char* binary = malloc(binary_len + 1);  // +1 for null terminator (for printing purposes only)
    
    // "10" start
    // binary[0] = '1';
    // binary[1] = '0';
    
    size_t bin_index = 0;
    // convert each char to 7-bit binary
    for (size_t i = 0; i < len; i++) {
        char ch = text[i];
        // Find the index of the char in ascii85_chars
        const char* char_pos = strchr(ascii85_chars, ch);
        if (char_pos == NULL) {
            // char not found in ascii85_chars
            printf("Invalid character: '%c', replaced with !\n", ch);  
            char_pos = ascii85_chars;
        }
        int char_index = char_pos - ascii85_chars;
        
        // Convert the index to 7-bit binary
        for (int j = 6; j >= 0; j--) {
            binary[bin_index++] = (char_index & (1 << j)) ? '1' : '0';
        }
    }
    
    // Add "111111" suffix
    //for (int i = 0; i < 7; i++) {
      //  binary[bin_index++] = '1';
   
    //}
    //binary[bin_index++] = '0'; // end

    binary[bin_index] = '\0';  // null terminator (for printing purposes)
    
    return binary;
}

char* binary_to_text(const char* binary) { 
    size_t binary_len = strlen(binary);

    // COMMENT FOR PRODUCTION
   //size_t text_len = (binary_len - 9) / 7;  // Subtract 9 for addtl bits, divide by 7 bits per char

	// UNCOMMENT FOR PRODUCTION 
    size_t text_len = (binary_len - 7) / 7;  // Subtract 7 for addtl bits, divide by 7 bits per char

    char* text = malloc(text_len + 1);  // +1 for null terminator
    
    // COMMENT FOR PRODUCTION - remove this in production as read code will deal with the "10"
    size_t bin_index = 0; // skip the first two bits ("10")
    
    for (size_t i = 0; i < text_len; i++) {
        char septet[8] = {0};
        strncpy(septet, binary + bin_index, 7);
        bin_index += 7;
        
        int value = strtol(septet, NULL, 2); // convert base two to a normal number

        text[i] = ascii85_chars[value];
    }
    
    text[text_len] = '\0';
    
    return text;
}

// -------------- HAMMING 7,4 ENCODING AND DECODING --------------

// based on algorithm outlined here: https://www.geeksforgeeks.org/hamming-code-in-computer-network/ 

// encoding inserts redundant bits in inputted string and then returns for transmission
// decoding checks for corruption, repairs if so, and then returns output sans redundant bits

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

// -------------- MAIN FOR TESTING --------------

// int main(){
//     char input[MAX_INPUT_LENGTH + 1];
//     printf("Enter text to send: ");
//     fgets(input,sizeof(input),stdin);

//     size_t input_len = strlen(input);
//     if (input[input_len - 1]=='\n'){
//         input[input_len - 1] = '\0';
//     }

//     char* binary = text_to_binary7(input);
//     char* bin_decode = binary_to_text7(binary);

//     // add hamming (7,4) bits
//     char* hamming_encode = hamming_encode_full(binary);
//     printf("Hamming Encode %s\n", hamming_encode);

//     // damage the hamming binary
//     hamming_encode[23] ^= 1;
//     printf("Fudged Hamming: %s\n", hamming_encode);

//     // decode hamming
//     char* hamming_decode = hamming_decode_full(hamming_encode);

//     // decode to text
//     char* text_decode = binary_to_text7(hamming_decode);

//     printf("Binary %s\n", binary);
//     // printf("Decoded Before Hamming %s\n", bin_decode);
//     printf("Decoded Text: %s\n", text_decode);
//     //send_bits(hamming_binary);
//     return 0;
// }
