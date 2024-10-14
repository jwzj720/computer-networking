#include "message_app.h"
#include "build_packet.h"
#include "hamming.h"
#include "read.h"
#include "send.h"
#include "selection.h"
#include "gui.h"
#include <pthread.h>
#include <ncurses.h>

pthread_t reading_thread;
pthread_t write_thread;
pthread_mutex_t send_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t read_mutex = PTHREAD_MUTEX_INITIALIZER;


struct AppData {
    int pinit;
    int selected_application;
    int send_rate;
    int message_size;
    struct timespec send_time;  // Store the time the message was sent
};

void send_response(struct AppData *app_data, uint8_t *received_data, size_t received_size)
{
    // Lock send mutex to ensure synchronized sending
    pthread_mutex_lock(&send_mutex);

    // Create response payload (for now, just echo the received message back)
    size_t data_size = received_size;
    uint8_t* payload = (uint8_t*)malloc(data_size);

    if (payload == NULL)
    {
        printf("Failed to allocate memory for response payload\n");
        pthread_mutex_unlock(&send_mutex);
        return;
    }

    // Copy received data into response payload (this is an echo, but you can modify it)
    memcpy(payload, received_data, data_size);

    // Send the response back (you can adjust this to use different rates, etc.)
    int eval = send_bytes(payload, data_size, GPIO_SEND, app_data->pinit, app_data->send_rate);

    if (eval != 0)
    {
        printf("Failed to send response message\n");
    }

    free(payload);  // Free payload after sending
    pthread_mutex_unlock(&send_mutex);
}

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

        struct timespec end_time;
        clock_gettime(CLOCK_MONOTONIC, &end_time);
        double elapsed_time = (end_time.tv_sec - app_data->send_time.tv_sec) * 1e9;
        elapsed_time = (elapsed_time + (end_time.tv_nsec - app_data->send_time.tv_nsec)) / 1e6;  // Convert to milliseconds
        printf("Message round-trip time: %.2f ms\n", elapsed_time);
        FILE *logfile = fopen("rtt_log.csv", "a");
        if (logfile) {
            fprintf(logfile, "%.2f\n", elapsed_time);
            fclose(logfile);
        }

        struct Packet* packet = generate_packet(rd->data);
        size_t decoded_len;
        
        // TODO: if app[0]:
        read_message(packet->data, packet->dlength, &decoded_len);

        send_response(app_data, packet->data, decoded_len);

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

void* send_thread(void* app_dati) // passing app_data in instead of pinit
{
    struct AppData *app_data = (struct AppData*) app_dati;

    while(1)
    {
        pthread_mutex_lock(&send_mutex);

        size_t data_size;
        uint8_t* payload = NULL;

        struct timespec start_time, end_time;
        //clock_gettime(CLOCK_MONOTONIC, &start_time);  // Start timing

        // IF chat
        if (app_data->selected_application == 0) // Chat application
        {
        printf("Text application\n");
	    fflush(stdin);
            payload = send_message(&data_size);
        }
        else if (app_data->selected_application == 1) // Pong application
        {
            printf("Pong application\n");
        }
        else {
            return NULL; // Invalid application
        }
        struct timespec start_time;  // Capture the sending time
        clock_gettime(CLOCK_MONOTONIC, &start_time);  // Start timing

        int eval = send_bytes(payload, data_size, GPIO_SEND, app_data->pinit, app_data->send_rate);
        if (eval != 0)
        {
            printf("Failed to send message\n");
            return NULL;
        }

        app_data->send_time = start_time;

        pthread_mutex_unlock(&send_mutex);
    }
    return NULL;
}


// baud rate, msg size, pigpio init=0
int main(int argc, char *argv[])
{
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <SEND_RATE> <MESSAGE_SIZE> <PIGPIO_INIT>\n", argv[0]);
        return 1;
    }

    // Initialize app data object
    struct AppData app_data;

    app_data.send_rate = atoi(argv[1]);      // Send rate passed from command line
    app_data.message_size = atoi(argv[2]);   // Message size passed from command line
    app_data.pinit = atoi(argv[3]);          // Pigpio initialization value from command line

    // Initialize router connection
    // Pass in pthread address so that 
    init_screen();

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

    // App Selection
    app_data.selected_application = app_select()-1; // Subtract 1 for offbyone error

    // Create reading/writing threads
    if(pthread_create(&reading_thread, NULL, read_thread, &app_data.pinit) != 0) {
        perror("Could not create reading thread");
        return 1;
    }
    time_sleep(.5);

    if(pthread_create(&write_thread, NULL, send_thread, &app_data) != 0) {
        perror("Could not create writing thread");
        return 1;
    }

    // Wait for writing to complete before continuing
    pthread_join(write_thread, NULL);

    pigpio_stop(app_data.pinit);

    return 0;
}
