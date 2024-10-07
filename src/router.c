// main.c
#include "encoding.h"
#include "transmission.h"
#include <pthread.h>
#include "read.h"
#include "build_packet.h"
#include "router.h"

pthread_t reading_thread;
pthread_t write_thread;

pthread_mutex_t send_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t read_mutex = PTHREAD_MUTEX_INITIALIZER;

/*
* function to relay the received message to specified destination.
*/
int relay()
{

}

int main()
{
    int pinit = pigpio_start(NULL, NULL);

    // Set GPIO modes
    set_mode(pinit, GPIO_SEND_1, PI_OUTPUT);
    set_mode(pinit, GPIO_RECEIVE_1, PI_INPUT);
    set_mode(pinit, GPIO_SEND_2, PI_OUTPUT);
    set_mode(pinit, GPIO_RECEIVE_2, PI_INPUT);

    if(pthread_create(&reading_thread, NULL, read_thread, &pinit) != 0) {
        perror("Could not create reading thread");
        return 1;
    }

    if(pthread_create(&write_thread, NULL, send_thread, &pinit) != 0) {
        perror("Could not create writing thread");
        return 1;
    }

    pthread_join(reading_thread, NULL);
    pthread_join(write_thread, NULL);

    pigpio_stop(pinit);

    return 0;
}
