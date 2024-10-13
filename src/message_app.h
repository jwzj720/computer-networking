#ifndef MESSAGE_APP_H
#define MESSAGE_APP_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "hamming.h"
#include "build_packet.h"
#include "selection.h"

#define MAX_INPUT_LENGTH 280


uint8_t* send_message(size_t* data_size);
void read_message(uint8_t* encoded_data, size_t encoded_len);

#endif /* MESSAGE_APP_H */