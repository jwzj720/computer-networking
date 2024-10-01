#ifndef HAMMING_H
#define HAMMING_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define DATA_BLOCK_SIZE 4
#define CODEWORD_SIZE 7

char* hamming_encode_full(char* binary_string);
char* hamming_decode_full(char* encoded_string);

#endif /*HAMMING_H*/