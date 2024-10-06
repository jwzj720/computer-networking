// main.c
#include "encoding.h"
#include "transmission.h"
#include <pthread.h>
#include "read.h"

pthread_t reading_thread;
pthread_t write_thread;

pthread_mutex_t send_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t read_mutex = PTHREAD_MUTEX_INITIALIZER;

void* read_thread(void* pinit)
{
    // Create Data reading object, which will store a message's data.
    create_reader(); // rd is now a global variable

    // Check data was allocated
    if (rd->data == NULL)
    {
        fprintf(stderr, "Couldn't allocate memory for data\n");
        return NULL;
    }

    // Register the callback without user data
    int id = callback(*(int*)pinit, GPIO_RECEIVE, EITHER_EDGE, get_bit);
    if (id < 0)
    {
        fprintf(stderr, "Failed to set callback\n");
        pigpio_stop(*(int*)pinit);
        return NULL;
    }

    while (1)
    {
        // read a message
        read_bits(rd);
        printf("Total bits read: %d\n", rd->counter);
        printf("Data buffer contents:\n");
        // Data received, lock threading to hold reading until packet is interpreted.
        pthread_mutex_lock(&read_mutex);
        struct Packet* packet = generate_packet(rd->data);

        if (packet != NULL)
        {
            print_packet_debug(packet->data, packet->dlength);
            free(packet->data);
            free(packet);
        }
        else
        {
            printf("Failed to generate packet.\n");
        }

        reset_reader(rd);
        pthread_mutex_unlock(&read_mutex);
    }

    callback_cancel(id);

    // Free Data
    free(rd->data);
    free(rd);

    return NULL;
}

void* send_thread(void* pinit)
{
    while(1)
    {
        //pthread_mutex_lock(&send_mutex);
        send_to_file(*(int*)pinit);
        //pthread_mutex_unlock(&send_mutex);
    }
    return NULL;
}

int main()
{

    int pinit = pigpio_start(NULL, NULL);

    if (pinit < 0)
    {
        fprintf(stderr, "Didn't initialize pigpio library\n");
        return 1; // Return NULL to indicate failure
    }

    // Set GPIO modes
    if (set_mode(pinit, GPIO_SEND, PI_OUTPUT) != 0)
    {
        fprintf(stderr, "Failed to set GPIO_SEND mode\n");
        pigpio_stop(pinit);
        return 1;
    }

    if (set_mode(pinit, GPIO_RECEIVE, PI_INPUT) != 0)
    {
        fprintf(stderr, "Failed to set GPIO_RECEIVE mode\n");
        pigpio_stop(pinit);
        return 1;
    }

    if(pthread_create(&reading_thread, NULL, read_thread, &pinit) != 0) {
        perror("Could not create reading thread");
        return 1;
    }
    time_sleep(0.1);

    if(pthread_create(&write_thread, NULL, send_thread, &pinit) != 0) {
        perror("Could not create writing thread");
        return 1;
    }

    pthread_join(reading_thread, NULL);
    pthread_join(write_thread, NULL);

    pigpio_stop(pinit);

    return 0;
}
