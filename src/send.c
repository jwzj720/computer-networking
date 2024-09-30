#include "send.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <pigpiod_if2.h>
#include <string.h>


int send_bits(char *bitstring) {
    
    int bitstring_length = strlen(bitstring);

    int pi = pigpio_start(NULL, NULL);
    if (pi < 0) {
        fprintf(stderr, "Failed to initialize pigpio library\n");
        return 1;
    }

    set_mode(pi, TX_PIN, PI_OUTPUT);

    int bit_time_us = 1000000 / BAUD_RATE;  // how we get time in microseconds
    int half_bit_time_us = bit_time_us / 2; 


    for (int i = 0; i < bitstring_length; i++) { // Error checking to see if the user didn't enter 1 or 0
        char bit_char = bitstring[i];
        if (bit_char != '0' && bit_char != '1') {
            fprintf(stderr, "Invalid bit '%c' at position %d. Use only '0' or '1'.\n", bit_char, i);
            pigpio_stop(pi);
            return 1;
        }

        int bit = bit_char - '0';

        // Logical '0': Low to High transition
        // Logical '1': High to Low transition

        if (bit == 0) {
            // 0: Low to High transition
            // First we set pin LOW
            gpio_write(pi, TX_PIN, 0);
            usleep(half_bit_time_us);

            // Second w set pin HIGH
            gpio_write(pi, TX_PIN, 1);
            usleep(half_bit_time_us);
        } else {
            // 1: High to Low transition

            // HIGH
            gpio_write(pi, TX_PIN, 1);
            usleep(half_bit_time_us);

            //LOW
            gpio_write(pi, TX_PIN, 0);
            usleep(half_bit_time_us);
        }
    }

   // gpio_write(pi, TX_PIN, 0);

    pigpio_stop(pi);
    return 0;
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <bitstring>\n", argv[0]);
        return 1;
    }

    char *bitstring = argv[1];
    int bitstring_length = strlen(bitstring);

    int pi = pigpio_start(NULL, NULL);
    if (pi < 0) {
        fprintf(stderr, "Failed to initialize pigpio library\n");
        return 1;
    }

    set_mode(pi, TX_PIN, PI_OUTPUT);

    int bit_time_us = 1000000 / BAUD_RATE;  // how we get time in microseconds
    int half_bit_time_us = bit_time_us / 2; 


    for (int i = 0; i < bitstring_length; i++) { // Error checking to see if the user didn't enter 1 or 0
        char bit_char = bitstring[i];
        if (bit_char != '0' && bit_char != '1') {
            fprintf(stderr, "Invalid bit '%c' at position %d. Use only '0' or '1'.\n", bit_char, i);
            pigpio_stop(pi);
            return 1;
        }

        int bit = bit_char - '0';

        

        if (bit == 0) {
           
            gpio_write(pi, TX_PIN, 0);
            usleep(half_bit_time_us);

            
            gpio_write(pi, TX_PIN, 1);
            usleep(half_bit_time_us);
        } else {

            gpio_write(pi, TX_PIN, 1);
            usleep(half_bit_time_us);


            gpio_write(pi, TX_PIN, 0);
            usleep(half_bit_time_us);
        }
    }

   // gpio_write(pi, TX_PIN, 0);

    pigpio_stop(pi);
    return 0;
}
