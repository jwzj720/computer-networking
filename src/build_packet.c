#include "build_packet.h"


int build_packet(uint8_t device_addr, uint8_t receiver_addr, uint8_t *data, size_t data_size, uint8_t *packet) {
    size_t offset = 0;

    
    memset(packet, 0, 50);  // 128 bytes is the maximum packet size

    uint16_t data_length = data_size;
    packet[offset++] = (data_length >> 8) & 0xFF; // Higher byte
    packet[offset++] = data_length & 0xFF;        // Lower byte


    packet[offset++] = device_addr;

    packet[offset++] = receiver_addr;

    memcpy(&packet[offset], data, data_size);
	
    return sizeof(packet);
}

// Function to print the packet in hex
void print_packet_debug(uint8_t *packet, size_t byte_size) {
    printf("Packet: ");
    for (size_t i = 0; i < byte_size; i++) {
        printf("%02X ", packet[i]);
    }
    printf("\n");
}

int compare_packets(uint8_t *packet1, uint8_t *packet2, size_t size) {
    for (size_t i = 0; i < size; i++) {
        if (packet1[i] != packet2[i]) {
            return 0;  // Packets don't match
        }
    }
    return 1;  // Packets match
}

// Unit test function
void test_build_packet() {
    // Given test data
    uint8_t device_addr =   0x01;  // Single 8-bit device address
    uint8_t receiver_addr = 0x09;  // Single 8-bit receiver address
    uint8_t data[4] =       {0xAA, 0xCC, 0xF0, 0x0F};  // 4 bytes of data

    uint8_t packet[128];
    size_t data_size = sizeof(data);

    build_packet(device_addr, receiver_addr, data, data_size, packet);

    uint8_t expected_packet[] = {
        0x00, 0x04, // Data length (4 bytes, 16 bits)
        0x01,       // Device address
        0x09,       // Receiver address
        0xAA,       // Data byte 1
        0xCC,       // Data byte 2
        0xF0,       // Data byte 3
        0x0F        // Data byte 4
    };

    printf("Expected packet:\n");
    print_packet_debug(expected_packet, sizeof(expected_packet));

    printf("Actual packet:\n");
    print_packet_debug(packet, sizeof(expected_packet));

    int result = compare_packets(packet, expected_packet, sizeof(expected_packet));

    if (result) {
        printf("Test passed: Packets match.\n");
    } else {
        printf("Test failed: Packets do not match.\n");
    }
}

//int main() {
    //Run the unit test
//    test_build_packet();

//    return 0;
//}
