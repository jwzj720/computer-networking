#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "text_6bit.h"


char* text_to_binary(const char* text) {
    size_t len = strlen(text);
    size_t binary_len = 2 + len * 6 + 6; // 2 for "10" + 6 bits per char + 6 for "111111"
    char* binary = malloc(binary_len + 1); // +1 for null terminator (for printing purposes only)
    
    // start with "10"
    binary[0] = '1';
    binary[1] = '0';
    
    size_t bin_index = 2;
    // convert each char to 6-bit binary
    for (size_t i = 0; i < len; i++) {
        char ch = text[i];
        // find the index of the char in base64_chars
        const char* char_pos = strchr(base64_chars, ch);
        if (char_pos == NULL) {
            // char not found in base64_chars -- use 'A' (000000) as replacement
            printf("Invalid character: '%c', replaced with A\n", ch);            
            char_pos = base64_chars;
        }
        int char_index = char_pos - base64_chars;
        
        // convert the index to 6-bit binary
        for (int j = 5; j >= 0; j--) {
            binary[bin_index++] = (char_index & (1 << j)) ? '1' : '0';
        }
    }
    
    // add "111111" to the end
    for (int i = 0; i < 6; i++) {
        binary[bin_index++] = '1';
    }
    
    binary[bin_index] = '\0';  // null terminator for printing
    
    return binary;
}

int prompt() {
    char input[MAX_INPUT_LENGTH + 1];
    printf("Enter text (max %d characters): ", MAX_INPUT_LENGTH);
    fgets(input, sizeof(input), stdin);

    // remove newline character
    size_t input_len = strlen(input);
    if (input[input_len - 1] == '\n') {
        input[input_len - 1] = '\0';
    }

    char* binary = text_to_binary(input);

    printf("Binary: %s\n", binary);

    return binary;
}