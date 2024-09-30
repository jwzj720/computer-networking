#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "text_7bit.h"

#define MAX_INPUT_LENGTH 280

char* text_to_binary7(const char* text) {
    size_t len = strlen(text);
    // 2 for "10" + 7 bits per char + 7 for "0000000"
    size_t binary_len = 2 + len * 7 + 7;
    char* binary = malloc(binary_len + 1);  // +1 for null terminator (for printing purposes only)
    
    // "10" start
    binary[0] = '1';
    binary[1] = '0';
    
    size_t bin_index = 2;
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
    for (int i = 0; i < 7; i++) {
        binary[bin_index++] = '1';
   
    }
    binary[bin_index++] = '0'; // end

    binary[bin_index] = '\0';  // null terminator (for printing purposes)
    
    return binary;
}

char* binary_to_text7(const char* binary) { 
    size_t binary_len = strlen(binary);

    // COMMENT FOR PRODUCTION
   // size_t text_len = (binary_len - 9) / 7;  // Subtract 9 for addtl bits, divide by 7 bits per char

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

// LOCAL TESTS
//int main() {
//    char input[MAX_INPUT_LENGTH + 1];
//    printf("Enter text (max %d characters): ", MAX_INPUT_LENGTH);
//    fgets(input, sizeof(input), stdin);
//
//    // remove newline character
//    size_t input_len = strlen(input);
//    if (input[input_len - 1] == '\n') {
//        input[input_len - 1] = '\0';
//    }
//
//    char* binary = text_to_binary7(input);
//
//    printf("Binary: %s\n", binary);
//
//    char* text = binary_to_text7(binary);
//
//    printf("Text: %s\n", text);
//
//    return 0;
//}
