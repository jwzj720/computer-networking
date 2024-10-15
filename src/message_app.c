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
int text_to_bytes(struct Packet* packet, char rec_name){
    // collect user input
    size_t* len = &packet->dlength;
    char input[MAX_INPUT_LENGTH + 1];
    fflush(stdin);
    printf("Please enter a message to send to %s: ", rec_name);
    fgets(input,sizeof(input),stdin);

    *len = strlen(input); // get length of input
    if (input[*len-1] == '\n') input[*len-1] = '\0';  // Remove newline character
    *len = strlen(input);

    // Allocate memory for the byte list (uint8_t array)
    // uint8_t* hexList = malloc(*len * sizeof(uint8_t));

    // convert each char to byte and add to the list
    for (size_t i = 0; i < *len; i++) {
        packet->data[i] = (uint8_t)input[i]; // Store ASCII as raw bytes
    }

    // for (size_t i = 0; i < *len; i++) {
    //     printf("%02X ", hexList[i]); // Print each byte in hex format
    // }
    // printf("<- Hex values");

    printf("\n");
    return 0;
    //return hexList; // return the byte array
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

int start_chat(struct AppData* app_data)
{
    send_message(app_data);
    printf("Message app started");
    read_message(app_data);
    return 0;
}

uint8_t* send_message(struct AppData* app_data)
{
    struct Packet* packet = app_data->sent_packet;
    packet->sending_addy = 0x01;
    char* receiver_name;
    packet->receiving_addy = select_address(&receiver_name);
    //print_byte_binary(receiver_addr);
    size_t payload_length;
    
    // Setting the data object should be sufficient to have data sent...
    text_to_bytes(packet, *receiver_name);

    //size_t encoded_length;
    //uint8_t* hamload = ham_encode(payload, payload_length, &encoded_length);

    //printf("Size of encoded packet %ld\n", encoded_length);

    //uint8_t* packet = (uint8_t*)malloc(50 * sizeof(uint8_t));
    //*data_size = build_packet(packet/*, hamload, encoded_length*/, payload);
    unlock(0); //unlock the write thread
    usleep(500);
    lock(0); //relock
    printf("Message sent successfully \n");
    return packet;
}

// void read_message(uint8_t* packet, size_t packet_len, size_t* decoded_len){
//     uint8_t* hamload = ham_decode(packet, packet_len, decoded_len);
//     char* message = bytes_to_text(hamload, *decoded_len);
//     printf("Message received: %s\n", message);
// }

void read_message(struct AppData* app_data){
    struct Packet* packet = app_data->received_packet;
  if(packet->dlength != 0)
  {
    char* message = bytes_to_text(packet->data,packet->dlength);
    printf("Message received: %s\n", message);
    free(message);
    packet->dlength = 0;
  }
}
