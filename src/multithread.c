// multithread.c

#include "message_app.h"
#include "build_packet.h"
#include "hamming.h"
#include "read.h"
#include "send.h"
#include "selection.h"
#include "gui.h"
#include "file_app.h"
#include <pthread.h>
#include <ncurses.h>

pthread_t reading_thread;
pthread_t write_thread;

pthread_mutex_t send_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t read_mutex = PTHREAD_MUTEX_INITIALIZER;

struct AppData {
    int pinit;
    int selected_application; // 0 for messaging, 1 for file transfer
    uint8_t device_addr;
    uint8_t receiver_addr;
    int out_pin;
    int in_pin;
};

void* read_thread(void* arg)
{
    struct AppData* app_data = (struct AppData*) arg;
    int pinit = app_data->pinit;

    // Create Data reading object, which will store a message's data.
    struct ReadData *rd = create_reader(1);
    
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

    // Create FileTransferApp instance
    FileTransferApp ft_app;
    FileTransferConfig config = {
        .pinit = pinit,
        .device_addr = app_data->device_addr,
        .receiver_addr = app_data->receiver_addr,
        .out_pin = app_data->out_pin,
        .in_pin = app_data->in_pin
    };
    FileTransferApp_init(&ft_app, config);

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
        
        if (app_data->selected_application == 0) // Messaging application
        {
            read_message(packet->data, packet->dlength, &decoded_len);
        }
        else if (app_data->selected_application == 1) // File transfer application
        {
            // Pass the packet data to FileTransferApp_receivePacket
            FileTransferApp_receivePacket(&ft_app, packet->data, packet->dlength);
        }
        else
        {
            printf("Invalid application selected.\n");
        }

        free(packet->data);
        free(packet);
        // Reset read variables each iteration.
        reset_reader(rd);
        pthread_mutex_unlock(&read_mutex);
    }

    // When done with the reading thread
    callback_cancel(id);

    // Free Data
    free(rd->data);
    free(rd);

    return NULL;
}

void* send_thread(void* arg)
{
    struct AppData *app_data = (struct AppData*) arg;

    // Create FileTransferApp instance
    FileTransferApp ft_app;
    FileTransferConfig config = {
        .pinit = app_data->pinit,
        .device_addr = app_data->device_addr,
        .receiver_addr = app_data->receiver_addr,
        .out_pin = app_data->out_pin,
        .in_pin = app_data->in_pin
    };
    FileTransferApp_init(&ft_app, config);

    while(1)
    {
        pthread_mutex_lock(&send_mutex);

        if (app_data->selected_application == 0) // Messaging application
        {
            printf("Text messaging application\n");
            fflush(stdin);
            size_t data_size;
            uint8_t* packet = send_message(&data_size);
            if (packet == NULL) {
                printf("Failed to prepare message\n");
                pthread_mutex_unlock(&send_mutex);
                continue;
            }

            int eval = send_bytes(packet, data_size, app_data->out_pin, app_data->pinit);
            if (eval != 0)
            {
                printf("Failed to send message\n");
                free(packet);
                pthread_mutex_unlock(&send_mutex);
                continue;
            }

            free(packet);
        }
        else if (app_data->selected_application == 1) // File transfer application
        {
            printf("File transfer application\n");
            FileTransferApp_sendFile(&ft_app);
	    //usleep(5000000);
        }
        else {
            printf("Invalid application selected.\n");
            pthread_mutex_unlock(&send_mutex);
            continue;
        }

        pthread_mutex_unlock(&send_mutex);
    }
    return NULL;
}

int main()
{
    // Initialize app data object
    struct AppData app_data;

    // Initialize GUI or any other setup if needed
    //init_screen();

    // Initialize pigpio
    app_data.pinit = pigpio_start(NULL, NULL);

    if (app_data.pinit < 0)
    {
        fprintf(stderr, "Didn't initialize pigpio library\n");
        return 1;
    }

    // Set GPIO modes
    app_data.out_pin = GPIO_SEND;
    app_data.in_pin = GPIO_RECEIVE;
    if (set_mode(app_data.pinit, app_data.out_pin, PI_OUTPUT) != 0 || 
        set_mode(app_data.pinit, app_data.in_pin, PI_INPUT) != 0)
    {
        pigpio_stop(app_data.pinit);
        return 1;
    }

    // Addresses (set your own device and receiver addresses)
    app_data.device_addr = 0x02;
    app_data.receiver_addr = select_address(NULL); // Implement this function or set a fixed address

    // App Selection
    app_data.selected_application = app_select() - 1; // Subtract 1 for zero-based index

    // Create reading/writing threads
    if(pthread_create(&reading_thread, NULL, read_thread, &app_data) != 0) {
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
    pthread_join(reading_thread, NULL);

    pigpio_stop(app_data.pinit);

    return 0;
}
