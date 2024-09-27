#ifndef TEXT_7BIT_H
#define TEXT_7BIT_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_INPUT_LENGTH 280

#define ascii85_chars "!\"#$&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwyxz "

char* text_to_binary7(const char* text);
char* binary_to_text7(const char* text);

#endif 