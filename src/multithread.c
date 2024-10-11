#include "message_app.h"
#include "build_packet.h"
#include "hamming.h"
#include "read.h"
#include "send.h"
#include "selection.h"
#include <pthread.h>

pthread_t reading_thread;
pthread_t write_thread;

pthread_mutex_t send_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t read_mutex = PTHREAD_MUTEX_INITIALIZER;


struct AppData {
    int pinit;
    int selected_application;
    int selected_recipient;  // not sure if we need this?
};


void* read_thread(void* pinit)
{
    // Create Data reading object, which will store a message's data.
    struct ReadData *rd = create_reader(1);
    
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
        
        // TODO: if app[0]:
        read_message(packet->data, packet->dlength, &decoded_len);

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

void* send_thread(void* pinit) // passing app_data in instead of pinit
{
    struct AppData *app_data = (struct AppData*) pinit;

    while(1)
    {
        pthread_mutex_lock(&send_mutex);

        size_t data_size;
        uint8_t* payload = NULL;

        // IF chat
        if (app_data->selected_application == 0) // Chat application
        {
            printf("Text application\n");
            payload = send_message(&data_size);
        }
        else if (app_data->selected_application == 1) // Pong application
        {
            printf("Pong application\n");
        }
        else {
            return NULL; // Invalid application
        }

        int eval = send_bytes(payload, data_size, GPIO_SEND, app_data->pinit);
        if (eval != 0)
        {
            printf("Failed to send message\n");
            return NULL;
        }

        pthread_mutex_unlock(&send_mutex);
    }
    return NULL;
}


int main()
{
    struct AppData app_data;

    // App Selection
    app_data.selected_application = select_application();

    // Initialize pigpio
    app_data.pinit = pigpio_start(NULL, NULL);

    if (app_data.pinit < 0)
    {
        fprintf(stderr, "Didn't initialize pigpio library\n");
        return 1;
    }

    // Set GPIO modes
    if (set_mode(app_data.pinit, GPIO_SEND, PI_OUTPUT) != 0 || 
        set_mode(app_data.pinit, GPIO_RECEIVE, PI_INPUT) != 0)
    {
        pigpio_stop(app_data.pinit);
        return 1;
    }

    if(pthread_create(&reading_thread, NULL, read_thread, &app_data.pinit) != 0) {
        perror("Could not create reading thread");
        return 1;
    }
    time_sleep(.5);

    if(pthread_create(&write_thread, NULL, send_thread, &app_data) != 0) {
        perror("Could not create writing thread");
        return 1;
    }

    pthread_join(reading_thread, NULL);
    pthread_join(write_thread, NULL);

    pigpio_stop(app_data.pinit);

    return 0;
}
