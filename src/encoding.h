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
#define ascii85_chars "!\"#$&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwyxz "

char* text_to_binary(const char* text);
char* binary_to_text(const char* text);
char* pack(char* ASCII_binary);
char* unpack(char* hamming_binary);
char* hamming_encode_full(char* packed_string);
char* hamming_decode_full(char* packed_string);

#endif 