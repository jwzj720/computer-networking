#ifndef PONG_H
#define PONG_H

#include "objects.h"

int start_pong(struct AppData* parent_data, pthread_mutex_t send_mutex, pthread_mutex_t read_mutex);
void send_update(uint8_t data);
uint8_t check_data();


#endif
