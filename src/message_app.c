// message_app.c

#include "message_app.h"
#include "hamming.h"
#include "selection.h"

#define MAX_INPUT_LENGTH 256  // Adjust as needed

uint8_t* text_to_bytes(size_t* len){
    // Collect user input
    char input[MAX_INPUT_LENGTH + 1];
    fflush(stdin);
    printf("Please enter a message to send: ");
    fgets(input, sizeof(input), stdin);

    *len = strlen(input);
    if (input[*len - 1] == '\n') input[*len - 1] = '\0';  // Remove newline
    *len = strlen(input);

    // Allocate memory for the byte list
    uint8_t* byte_list = malloc(*len * sizeof(uint8_t));
    if (!byte_list) {
        fprintf(stderr, "Memory allocation failed in text_to_bytes.\n");
        *len = 0;
        return NULL;
    }

    // Convert each character to byte
    for (size_t i = 0; i < *len; i++) {
        byte_list[i] = (uint8_t)input[i];
    }

    return byte_list;
}

char* bytes_to_text(const uint8_t* bytes, size_t len){
    char* text_out = malloc(len + 1);
    if (!text_out) {
        fprintf(stderr, "Memory allocation failed in bytes_to_text.\n");
        return NULL;
    }

    for (size_t i = 0; i < len; i++) {
        text_out[i] = (char)bytes[i];
    }

    text_out[len] = '\0';

    return text_out;
}

uint8_t* send_message(size_t* data_size, uint8_t* recipient_id) {
    // Select recipient
    *recipient_id = select_recipient();

    size_t payload_length;
    uint8_t* payload = text_to_bytes(&payload_length);

    if (!payload || payload_length == 0) {
        fprintf(stderr, "No message entered.\n");
        *data_size = 0;
        return NULL;
    }

    // Apply Hamming encoding to the payload
    size_t encoded_length;
    uint8_t* encoded_payload = ham_encode(payload, payload_length, &encoded_length);
    free(payload); // Free the original payload

    if (!encoded_payload || encoded_length == 0) {
        fprintf(stderr, "Failed to encode message.\n");
        *data_size = 0;
        return NULL;
    }

    *data_size = encoded_length;
    return encoded_payload;
}

void read_message(uint8_t* encoded_data, size_t encoded_len) {
    size_t decoded_len;
    uint8_t* decoded_data = ham_decode(encoded_data, encoded_len, &decoded_len);

    if (!decoded_data || decoded_len == 0) {
        fprintf(stderr, "Failed to decode the message or message is empty.\n");
        return;
    }

    char* message = bytes_to_text(decoded_data, decoded_len);
    free(decoded_data); // Free decoded data

    if (!message) {
        fprintf(stderr, "Failed to convert decoded data to text.\n");
        return;
    }

    printf("Message received: %s\n", message);
    free(message); // Free message string
}
