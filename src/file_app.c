// FileTransferApp.c

#include "file_app.h"

static uint8_t* file_to_bytes(const char* filename, size_t* len) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    *len = ftell(file);
    rewind(file);

    // Allocate memory for the file content
    uint8_t* buffer = malloc(*len);
    if (!buffer) {
        perror("Memory allocation failed");
        fclose(file);
        return NULL;
    }

    // Read file into buffer
    size_t read_len = fread(buffer, 1, *len, file);
    if (read_len != *len) {
        perror("Error reading file");
        fclose(file);
        free(buffer);
        return NULL;
    }

    fclose(file);
    return buffer;
}

static void write_bytes_to_file(const char* filename, uint8_t* data, size_t len) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        perror("Error opening file for writing");
        return;
    }

    fwrite(data, 1, len, file);
    fclose(file);
}

void FileTransferApp_init(FileTransferApp* app, FileTransferConfig config) {
    app->config = config;
}

void FileTransferApp_sendFile(FileTransferApp* app) {
    // Prompt user for filename
    char filename[256];
    printf("Please enter the filename to send: ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = '\0'; // Remove newline character

    size_t file_length;
    uint8_t* file_data = file_to_bytes(filename, &file_length);
    if (!file_data) {
        // Handle error (file couldn't be read)
        return;
    }

    // Calculate number of packets
    size_t total_packets = (file_length + MAX_PAYLOAD_SIZE - 1) / MAX_PAYLOAD_SIZE;

    for (size_t packet_num = 0; packet_num < total_packets; ++packet_num) {
        size_t offset = packet_num * MAX_PAYLOAD_SIZE;
        size_t payload_length = ((file_length - offset) > MAX_PAYLOAD_SIZE) ? MAX_PAYLOAD_SIZE : (file_length - offset);

        uint8_t* payload = &file_data[offset];

        // Include sequence number in the payload
        uint8_t seq_num = (uint8_t)packet_num; // Assuming total packets <= 256

        size_t packet_payload_length = payload_length + 1; // +1 for sequence number
        uint8_t* packet_payload = malloc(packet_payload_length);
        packet_payload[0] = seq_num;
        memcpy(&packet_payload[1], payload, payload_length);

        size_t encoded_length;
        uint8_t* hamload = ham_encode(packet_payload, packet_payload_length, &encoded_length);

        uint8_t packet[50];
        size_t packet_size = build_packet(
            app->config.device_addr,
            app->config.receiver_addr,
            hamload,
            encoded_length,
            packet
        );

        // Send the packet using existing send_bytes function
        int eval = send_bytes(packet, packet_size, app->config.out_pin, app->config.pinit);
        if (eval != 0) {
            printf("Failed to send packet %zu\n", packet_num);
            // Handle error appropriately
        }

        free(packet_payload);
        free(hamload);
    }

    free(file_data);

    printf("File sent successfully\n");
}

void FileTransferApp_receivePacket(FileTransferApp* app, uint8_t* packet_data, size_t packet_len)
{
    static uint16_t expected_packets = 256; // Adjust as needed
    static uint16_t total_received = 0;
    static uint8_t** received_packets = NULL;
    static size_t* packet_lengths = NULL;
    static int* packet_received_flags = NULL;
    static size_t total_length = 0;

    if (received_packets == NULL) {
        received_packets = calloc(expected_packets, sizeof(uint8_t*));
        packet_lengths = calloc(expected_packets, sizeof(size_t));
        packet_received_flags = calloc(expected_packets, sizeof(int));
    }

    size_t decoded_len;
    uint8_t* hamload = ham_decode(packet_data, packet_len, &decoded_len);
    if (hamload == NULL) {
        printf("Failed to decode packet\n");
        return;
    }

    // Extract sequence number
    uint8_t seq_num = hamload[0];
    size_t payload_length = decoded_len - 1;

    if (seq_num >= expected_packets) {
        printf("Invalid sequence number: %u\n", seq_num);
        free(hamload);
        return;
    }

    if (packet_received_flags[seq_num]) {
        // Packet already received
        free(hamload);
        return;
    }

    // Store the payload
    received_packets[seq_num] = malloc(payload_length);
    memcpy(received_packets[seq_num], &hamload[1], payload_length);
    packet_lengths[seq_num] = payload_length;
    packet_received_flags[seq_num] = 1;
    total_received++;
    total_length += payload_length;

    free(hamload);

    // Check if all packets have been received
    if (total_received >= expected_packets) {
        // Reconstruct the file
        uint8_t* file_data = malloc(total_length);
        size_t offset = 0;
        for (size_t i = 0; i < expected_packets; ++i) {
            if (packet_received_flags[i]) {
                memcpy(&file_data[offset], received_packets[i], packet_lengths[i]);
                offset += packet_lengths[i];
                free(received_packets[i]);
            } else {
                printf("Missing packet %zu\n", i);
                // Handle missing packet (e.g., request retransmission)
            }
        }

        // Write the file
        char output_filename[256];
        printf("Please enter the output filename: ");
        fgets(output_filename, sizeof(output_filename), stdin);
        output_filename[strcspn(output_filename, "\n")] = '\0';

        write_bytes_to_file(output_filename, file_data, total_length);

        printf("File received and saved as %s\n", output_filename);

        // Free resources
        free(file_data);
        free(received_packets);
        free(packet_lengths);
        free(packet_received_flags);

        // Reset static variables
        received_packets = NULL;
        packet_lengths = NULL;
        packet_received_flags = NULL;
        total_received = 0;
        total_length = 0;
    }
}
