#include "message_app.h"
#include "build_packet.h"
#include "hamming.h"
#include <pthread.h>
#include "read.h"

pthread_t reading_thread;
pthread_t write_thread;

pthread_mutex_t send_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t read_mutex = PTHREAD_MUTEX_INITIALIZER;

int select_application(APPLICATION_LIST){

    // iterate through apps, print options

    // get user input on which application they would like

    // return desired application
}

int select_address(ADDRESS_BOOK){

    // iterate through dictionary, print options

    // get user input on who to send to

    // return user desired receivers address
}

void* read_thread(void* pinit)
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
    int id = callback_ex(*(int*)pinit, GPIO_RECEIVE, EITHER_EDGE, get_bit, rd);
    if (id < 0)
    {
        fprintf(stderr, "Failed to set callback\n");
        pigpio_stop(*(int*)pinit);
        
        return NULL;
    }

    // THIS is the per message read loop
    while(1)
    {
        // read a message
        read_bits(rd);
        
        // Data received, lock threading to hold reading until packet is interpreted.
        // We don't want rd->data to be overwritten during this time.
        pthread_mutex_lock(&read_mutex);
        struct Packet* packet = generate_packet(rd->data);
        size_t decoded_len;
        uint8_t* decoded_packet = ham_decode(packet->data, packet->dlength, &decoded_len);
        print_packet_debug(packet->data,packet->dlength);
        char* message = bytes_to_text(decoded_packet, decoded_len);
        printf("Message received: %s\n", message);

	    free(packet->data);
	    free(packet);
        //reset readrate and run variables each iteration.
        reset_reader(rd);
        pthread_mutex_unlock(&read_mutex);
        
    }

    //When done with the reading thread
    callback_cancel(id);

    // Free Data
    free(rd->data); //Do we need to free the data? pretty sure this is done in the read_to_file.
    free(rd);

    return NULL;
}

void* send_thread(void* pinit)
{
    while(1)
    {
        pthread_mutex_lock(&send_mutex);

        //TODO: if message app selected
        int payload = send_message(*(int*)pinit)

        pthread_mutex_unlock(&send_mutex);
    }
    return NULL;
}

int main()
{
    // print init stuff

    // Bitties OS

    // Explanation

    // App Selection

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
    time_sleep(.5);

    if(pthread_create(&write_thread, NULL, send_thread, &pinit) != 0) {
        perror("Could not create writing thread");
        return 1;
    }

    pthread_join(reading_thread, NULL);
    pthread_join(write_thread, NULL);

    pigpio_stop(pinit);

    return 0;
}
