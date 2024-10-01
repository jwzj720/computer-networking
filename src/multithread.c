// main.c
#include "link_receive.h"
#include "link_send.h"
#include <pthread.h>
#include "read.h"

pthread_t reading_thread;
pthread_t write_thread;

pthread_mutex_t send_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t read_mutex = PTHREAD_MUTEX_INITIALIZER;

void* read_thread(int* pinit)
{
    // Create Data reading object, which will store a message's data.
    struct ReadData *rd = create_reader();
    
    // Check data was allocated
    if (rd->data == NULL)
    {
        fprintf(stderr, "Couldn't allocate memory for data\n");
        return NULL;
    }
    // Register the callback with user data
    int id = callback_ex(pinit, GPIO_RECEIVE, EITHER_EDGE, get_bit, rd);
    if (id < 0)
    {
        fprintf(stderr, "Failed to set callback\n");
        pigpio_stop(pinit);
        
        return NULL;
    }

    

    // THIS is the per message read loop
    while(1)
    {
        // read a message
        read_message(rd);
        //pthread_mutex_lock(&read_mutex);
        //reset readrate and run variables each iteration.
        reset_reader(rd);
        //pthread_mutex_unlock(&read_mutex);
        
    }

    //When done with the reading thread
    callback_cancel(id);

    // Free Data
    free(rd->data);
    free(rd);

    

    return NULL;
}

void* send_thread(void* arg)
{
    while(1)
    {
        //pthread_mutex_lock(&send_mutex);
        send_to_file();
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
        return NULL; // Return NULL to indicate failure
    }

    // Set GPIO modes
    if (set_mode(pinit, GPIO_SEND, PI_OUTPUT) != 0)
    {
        fprintf(stderr, "Failed to set GPIO_SEND mode\n");
        pigpio_stop(pinit);
        return NULL;
    }

    if (set_mode(pinit, GPIO_RECEIVE, PI_INPUT) != 0)
    {
        fprintf(stderr, "Failed to set GPIO_RECEIVE mode\n");
        pigpio_stop(pinit);
        return NULL;
    }

    if(pthread_create(&reading_thread, NULL, read_thread, &pinit) != 0) {
        perror("Could not create reading thread");
        return 1;
    }

    if(pthread_create(&write_thread, NULL, send_thread, NULL) != 0) {
        perror("Could not create writing thread");
        return 1;
    }

    pthread_join(reading_thread, NULL);
    pthread_join(write_thread, NULL);

    pigpio_stop(pinit);

    return 0;
}
