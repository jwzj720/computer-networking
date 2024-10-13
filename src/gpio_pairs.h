// gpio_pairs.h

#ifndef GPIO_PAIRS_H
#define GPIO_PAIRS_H

#define NUM_GPIO_PAIRS 4

struct GPIO_Pair {
    int gpio_in;
    int gpio_out;
    uint8_t connected_device_id; 
};

extern struct GPIO_Pair gpio_pairs[NUM_GPIO_PAIRS];

#endif /* GPIO_PAIRS_H */