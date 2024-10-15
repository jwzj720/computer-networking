// Application to send and receive written text

#include "message_app.h"

/* Converts a string of ASCII text to an array of hex values

This function takes input from the user, converts each character into its corresponding
ASCII value, and stores these values as bytes (uint8_t) in a dynamically allocated array

INPUT:
len: Pointer to store the length of the byte array

OUTPUT:
uint8_t* hexList: A dynamically allocated byte array containing the ASCII values of each character in the input string.
Each byte represents one character from the input string in its ASCII hex value
*/
uint8_t* text_to_bytes(size_t* len, char* message){
    // collect user input
    char input[MAX_INPUT_LENGTH + 1];
    strncpy(input, message, MAX_INPUT_LENGTH);
    input[MAX_INPUT_LENGTH] = '\0';
    //fflush(stdin);
    //printf("Please enter a message to send to %s: ", rec_name);
    //fgets(input,sizeof(input),stdin);


    *len = strlen(input); // get length of input
    if (input[*len-1] == '\n') input[*len-1] = '\0';  // Remove newline character
    *len = strlen(input);

    // Allocate memory for the byte list (uint8_t array)
    uint8_t* hexList = malloc(*len * sizeof(uint8_t));

    // convert each char to byte and add to the list
    for (size_t i = 0; i < *len; i++) {
        hexList[i] = (uint8_t)input[i]; // Store ASCII as raw bytes
    }

    // for (size_t i = 0; i < *len; i++) {
    //     printf("%02X ", hexList[i]); // Print each byte in hex format
    // }
    // printf("<- Hex values");

    printf("\n");

    return hexList; // return the byte array
}

/* Converts an array of hex values back to a string of ASCII text

This function takes an array of bytes (uint8_t array), interprets each byte as an ASCII
character, and reconstructs the original string of text from the array

INPUTS:
const uint8_t bytes: A pointer to the byte array containing the ASCII values
size_t len: Length of the byte array

OUTPUT:
char* text_out: string containing the reconstructed ASCII text
*/
char* bytes_to_text(const uint8_t* bytes, size_t len){
    char* text_out = malloc(len+1);

    for (size_t i = 0; i < len; i++) {
        text_out[i] = (char)bytes[i];  // convert each byte to a char
    }

    text_out[len] = '\0'; 

    return text_out;
}

uint8_t* send_message(size_t* data_size, char* message)
{
    uint8_t device_addr = 0x1B; // GPIO PIN HARDCODE
    // char* receiver_name;
    uint8_t receiver_addr = 0x1A; //select_address(&receiver_name);
    //print_byte_binary(receiver_addr);
    size_t payload_length;
    uint8_t* payload = text_to_bytes(&payload_length, message);

    size_t encoded_length;
    uint8_t* hamload = ham_encode(payload, payload_length, &encoded_length);

    //printf("Size of encoded packet %ld\n", encoded_length);

    uint8_t* packet = (uint8_t*)malloc(50 * sizeof(uint8_t));
    *data_size = build_packet(device_addr, receiver_addr, hamload, encoded_length, packet);
    
    printf("Message sent successfully \n");
    return packet;
}

void read_message(uint8_t* packet, size_t packet_len, size_t* decoded_len){
    uint8_t* hamload = ham_decode(packet, packet_len, decoded_len);
    char* message = bytes_to_text(hamload, *decoded_len);
    printf("Message received: %s\n", message);
}
