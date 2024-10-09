#include "message_app.h"
#include "hamming.h"
#include "read.h"
#include "send.h"
#include "selection.h"
#include "gui.h"
#include "pong_game.h"

pthread_t reading_thread;
pthread_t write_thread;

pthread_mutex_t send_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t read_mutex = PTHREAD_MUTEX_INITIALIZER;


void* read_thread(void* pinit)
{
    // Allocate memory for the packet object... Still need to allcate the packet data memory.
    struct AppData *app_data = (struct AppData*) pinit;
    app_data->received_packet = malloc(sizeof(struct Packet));
    // Create Data reading object, which will store a message's data.
    struct ReadData *rd = create_reader(1);

    struct Packet* packet = app_data->received_packet;
    packet->data = malloc(sizeof(uint8_t)*50);
    packet->data[0]=0x00;
    
    // Check data was allocated
    if (rd->data == NULL)
    {
        fprintf(stderr, "Couldn't allocate memory for data\n");
        return NULL;
    }
    // Register the callback with user data
    int id = callback_ex(app_data->pinit, GPIO_RECEIVE, EITHER_EDGE, get_bit, rd);
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
        data_to_packet(packet, rd->data);

        //reset readrate and run variables each iteration.
        reset_reader(rd);
        pthread_mutex_unlock(&read_mutex);
    }

    //When done with the reading thread
    callback_cancel(id);

    // Free Data
    free(rd->data); //Do we need to free the data? pretty sure this is done in the read_to_file.
    free(rd);
    free(packet->data);
    free(packet);

    return NULL;
}

void* send_thread(void* pinit) // passing app_data in instead of pinit
{
    struct AppData *app_data = (struct AppData*) pinit;
    // Allocate memory for the packet object... Still need to allcate the packet data memory.
    app_data->sent_packet = malloc(sizeof(struct Packet));
    struct Packet* packet_data = app_data->sent_packet;
    packet_data->data = (uint8_t*)calloc(50,sizeof(uint8_t));

    // Allocate payload memory
    uint8_t* payload = (uint8_t*)malloc(50 * sizeof(uint8_t));

    // Wont start sending until thread first locked.

    while(1)
    {
        // Lock the send data while being sent...
        pthread_mutex_lock(&send_mutex);

        size_t pack_size;
        
        // put sendable data into payload variable and define packet size
        pack_size = build_packet(packet_data, payload);

        // Send the message
        int eval = send_bytes(payload, pack_size, GPIO_SEND, app_data->pinit);
        if (eval != 0)
        {
            printf("Failed to send message\n");
            return NULL;
        }
        // free packet data memory
        free(packet_data->data);
        packet_data->dlength=0;
        // unlock so it can be repopulated.
        pthread_mutex_unlock(&send_mutex);
    }
    return NULL;
}

int main()
{

    // Initialize router connection
    // Pass in pthread address so that 
    init_screen();

    // Initialize app data object
    struct AppData app_data;

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

    // Start send thread locked...
    pthread_mutex_lock(&send_mutex);

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
    // Main Running loop
    while (1)
    {
        app_data.selected_application = app_select()-1;
        if (app_data.selected_application == 0) // Chat application
            {
                //start_message(&app_data); //Run the message app
                printf("Text application\n");
		usleep(5000000);
                fflush(stdin);
                // payload = send_message(&data_size);
            }
            else if (app_data.selected_application == 1) // Pong application
            {
                start_pong(&app_data, send_mutex, read_mutex);
            }
            else {
                return 0; // Invalid application
            }
        // When app is exited and returns, go back to the app select screen.

    }
    // Wait for writing to complete before continuing
    pthread_join(write_thread, NULL);
    pthread_join(reading_thread, NULL);

    pigpio_stop(app_data.pinit);

    return 0;
}
