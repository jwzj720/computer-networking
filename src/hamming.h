#ifndef ENCODING_H
#define ENCODING_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

uint8_t* ham_encode(uint8_t* payload, size_t len, size_t* encoded_len);
uint8_t* ham_decode(uint8_t* encoded_array, size_t encoded_len, size_t* decoded_len);

#endif