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

uint8_t text_to_bytes();
char* bytes_to_text();
char* pack(char* ASCII_binary);
char* unpack(char* hamming_binary);
char* hamming_encode_full(char* packed_string);
char* hamming_decode_full(char* packed_string);

#endif 