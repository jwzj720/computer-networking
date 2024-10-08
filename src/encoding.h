#ifndef ENCODING_H
#define ENCODING_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define MAX_INPUT_LENGTH 280
#define DATA_BLOCK_SIZE 4
#define CODEWORD_SIZE 8

uint8_t* text_to_bytes(size_t* length);
char* bytes_to_text(const uint8_t* bytes, size_t len);
uint8_t* ham_encode(uint8_t* payload, size_t len, size_t* encoded_len);
uint8_t* ham_decode(uint8_t* encoded_array, size_t encoded_len, size_t* decoded_len);

#endif 
