#include "send.h"

void header(int out_pin, int pi, int half_bit_time_us)
{
    // 1
    gpio_write(pi, out_pin, 1);  // Set pin HIGH
    usleep(half_bit_time_us);   // Sleep for half bit time

    gpio_write(pi, out_pin, 0);  // Set pin LOW
    usleep(half_bit_time_us);   // Sleep for the remaining half bit time

    // 0
    gpio_write(pi, out_pin, 0);  // Set pin LOW
    usleep(half_bit_time_us);   // Sleep for half bit time

    gpio_write(pi, out_pin, 1);  // Set pin HIGH
    usleep(half_bit_time_us);   // Sleep for the remaining half bit time
}

void tail(int out_pin, int pi, int half_bit_time_us)
{
    // 1's
    for (int i = 0; i < 8; i++) {
        // 1: High to Low transition
        gpio_write(pi, out_pin, 1);  // Set pin HIGH
        usleep(half_bit_time_us);   // Sleep for half bit time

        gpio_write(pi, out_pin, 0);  // Set pin LOW
        usleep(half_bit_time_us);   // Sleep for the remaining half bit time
    }
    // 0
    gpio_write(pi, out_pin, 0);  // Set pin LOW
    usleep(half_bit_time_us);   // Sleep for half bit time
    
    gpio_write(pi, out_pin, 1);  // Set pin LOW
    usleep(half_bit_time_us);

}
int send_bytes(uint8_t *packet, size_t packet_size, int out_pin, int pi) {
    int bit_time_us = 1000000 / BAUD_RATE; 
    int half_bit_time_us = bit_time_us / 2;
    
    header(out_pin, pi, half_bit_time_us);
    // prepend 10 at the beginning of the packet and append 11111110 at the end
    for (size_t i = 0; i < packet_size; i++) {
        uint8_t byte = packet[i];
        for (int bit_pos = 7; bit_pos >= 0; bit_pos--) {
            int bit = (byte >> bit_pos) & 0x01;  // Extract the bit from the current byte

            // Logical '0': Low to High transition
            // Logical '1': High to Low transition

            if (bit == 0) {
                // 0: Low to High transition
                gpio_write(pi, out_pin, 0);  // Set pin LOW
                usleep(half_bit_time_us);   // Sleep for half bit time

                gpio_write(pi, out_pin, 1);  // Set pin HIGH
                usleep(half_bit_time_us);   // Sleep for the remaining half bit time
            } else {
                // 1: High to Low transition
                gpio_write(pi, out_pin, 1);  // Set pin HIGH
                usleep(half_bit_time_us);   // Sleep for half bit time

                gpio_write(pi, out_pin, 0);  // Set pin LOW
                usleep(half_bit_time_us);   // Sleep for the remaining half bit time
            }
        }
    }
    tail(out_pin, pi, half_bit_time_us);

    return 0;
}


