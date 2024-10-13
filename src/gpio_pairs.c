// gpio_pairs.c

#include "gpio_pairs.h"

struct GPIO_Pair gpio_pairs[NUM_GPIO_PAIRS] = {
    {26, 27}, // Pair 1: gpio_in, gpio_out
    {24, 25}, // Pair 2
    {22, 23}, // Pair 3
    {20, 21}  // Pair 4
};
