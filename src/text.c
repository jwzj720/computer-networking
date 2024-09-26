#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_INPUT_LENGTH 280

const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789. ";
const char ascii85_chars[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.-:+=^!/*?&<>()[]{}@$# ";

char* text_to_binary(const char* text) {
    size_t len = strlen(text);
    size_t binary_len = 2 + len * 7 + 6; // 2 for "10" + 7 bits per char + 6 for "111111"
    char* binary = malloc(binary_len + 1); // +1 for null terminator (for printing purposes only)
    
    // start with "10"
    binary[0] = '1';
    binary[1] = '0';
    
    size_t bin_index = 2;
    // convert each char to 7-bit binary
    for (size_t i = 0; i < len; i++) {
        char ch = text[i];
        // find the index of the char in base64_chars
        // const char* char_pos = strchr(base64_chars, ch);

        // find the index of the char in ascii85_chars
        const char* char_pos = strchr(ascii85_chars, ch);

        if (char_pos == NULL) {
            // char not found in base64_chars -- use 'A' (000000) as replacement
            printf("Invalid character: '%c', replaced with A\n", ch);            
            // char_pos = base64_chars;
            char_pos = ascii85_chars;
        }
        // int char_index = char_pos - base64_chars;
        int char_index = char_pos - ascii85_chars;

        // convert the index to 6-bit binary
        for (int j = 6; j >= 0; j--) {
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


char* binary_to_text(const char* numbers){
    // TODO

    // disregard the starting two bits '10'

    // parse through each
    return 0;
}

int main() {
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

    free(binary);
    return 0;
}