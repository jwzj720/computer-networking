#include <stdlib.h>
#include <stdio.h>
// #include <pigpiod_if2.h>
#include <string.h>
#include <inttypes.h>

#include "read.h"
// #include "send.h"
// include "text_6bit.h"
#include "text_7bit.h"
#include "hamming.h"

int send_to_file()
{
    char input[MAX_INPUT_LENGTH + 1];
    printf("Enter text to send: ");
    fgets(input,sizeof(input),stdin);

    size_t input_len = strlen(input);
    if (input[input_len - 1]=='\n'){
        input[input_len - 1] = '\0';
    }

    char* binary = text_to_binary7(input);

    // add hamming (7,4) bits
    char* hamming_binary = hamming_encode_full(binary);

    printf("Binary %s\n", binary);
    printf("Binary %s\n", hamming_binary);
    send_bits(hamming_binary);
    return 0;
}

// testing grounds
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
