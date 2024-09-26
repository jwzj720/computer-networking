#include <stdio.h>
#include <stdlib.h>
#include <pigpiod_if2.h>
#include <unistd.h>

#define RX_PIN 26 
#define BAUD_RATE 50

typedef struct {
    uint32_t last_tick;
    unsigned last_edge;
    int bit_time_us;
    int half_bit_time_us;
    int tolerance;
    int receiving;
} decoder_state_t;

void call_back(unsigned gpio, unsigned edge, uint32_t tick, void *userdata);

int main() {
    int pi = pigpio_start(NULL, NULL);
    if (pi < 0) {
        fprintf(stderr, "Failed to initialize pigpio library\n");
        return 1;
    }

    set_mode(pi, RX_PIN, PI_INPUT);
    set_pull_up_down(pi, RX_PIN, PI_PUD_OFF); // clears it 

    decoder_state_t decoder_state = {0};
    decoder_state.last_tick = 0;
    decoder_state.last_edge = 2;
    decoder_state.bit_time_us = 1000000 / BAUD_RATE;
    decoder_state.half_bit_time_us = decoder_state.bit_time_us / 2;
    decoder_state.tolerance = decoder_state.half_bit_time_us / 2; // 50% tolerance
    decoder_state.receiving = 0;

    // Use the callback_ex function to pass the decoder
    int cb_id = callback_ex(pi, RX_PIN, EITHER_EDGE, call_back, &decoder_state);
    
    if (cb_id < 0) {
        fprintf(stderr, "Failed to set up callback\n");
        pigpio_stop(pi);
        return 1;
    }

    printf("Waiting for data...\n");

    while (1) {
        time_sleep(1.0);
    }

    pigpio_stop(pi);
    return 0;
}

void call_back(unsigned gpio, unsigned edge, uint32_t tick, void *userdata) {
    if (edge == 2) {
        return;
    }

    decoder_state_t *state = (decoder_state_t *)userdata;

    // If this is the first edge, initialize last_tick
    if (state->last_tick == 0) {
        state->last_tick = tick;
        state->last_edge = edge;
        return;
    }

    // Calculate time since last edge
    uint32_t dt = tick - state->last_tick;

    if (dt >= (state->half_bit_time_us - state->tolerance) && dt <= (state->half_bit_time_us + state->tolerance)) {
        // Edge occurred after ~ half of a bit time

        // Determine the bit value based on transition
        if (state->last_edge == FALLING_EDGE && edge == RISING_EDGE) {
            // Low-to-High transition represents '0'
            printf("0");
            fflush(stdout);
        } else if (state->last_edge == RISING_EDGE && edge == FALLING_EDGE) {
            // High-to-Low transition represents '1'
            printf("1");
            fflush(stdout);
        } else {
            // Not a good transition
        }
    } else if (dt > (state->bit_time_us + state->tolerance)) {
        // this would be the case where we have a gap in the transmission, maybe we print a new line?
        printf("\n");
        fflush(stdout);
    }

    // Update state
    state->last_tick = tick;
    state->last_edge = edge;
}
