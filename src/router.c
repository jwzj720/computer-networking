// main.c
#include "send.h"
#include "encoding.h"
#include <pthread.h>
#include "read.h"
#include "build_packet.h"
#include "router.h"

#define BOOKSIZE 3

struct User* addressBook[BOOKSIZE]; /* pointer table */

pthread_t tid[BOOKSIZE];
pthread_t write_thread;

pthread_mutex_t lock;

int pinit; // Variable for GPIO status.

/* HELPER FUNCTIONS */

// Right now lookup acts like a linked list, goes through every value
// Could be implemented to act like hash table with faster lookup
struct User *lookup(uint8_t s)
{
    struct User *np;
    for (np = addressBook[0]; np != NULL; np = np->next)
        if (s == np->ID)
          return np; // found
    return NULL; // not found 
}

/*
* function to relay the received message to specified destination.
*/
int relay(struct Packet* pack)
{


    // Find the destination for packet in the lookup table
    struct User* dest;
    if ((dest = lookup(pack->receiving_addy)) == NULL){
        fprintf(stderr, "Intended recipient not known by router.\n");
        return 1;
    };
    uint8_t temp_pack[50];
    // Generate a new packet to be sent to next user
    int packet_size = build_packet(pack->sending_addy, pack->receiving_addy, pack->data, sizeof(pack->data), temp_pack);

    //Send the packet!
    // This currently expects a different packet object. Need to generalize with packet struct.
    if (send_bytes(temp_pack, packet_size, dest->GPIO_OUT, pinit) != 0)
    {
        fprintf(stderr, "Failed to send packet\n");
        return 1;
    }
    return 0;

}

void* read_thread(void* port)
{

    struct User* user = (struct User*) port;
    // Create Data reading object, which will store a message's data.
    struct ReadData *rd = create_reader(user->ID);
    
    // Register the callback with user data
    int id = callback_ex(pinit, user->GPIO_IN, EITHER_EDGE, get_bit, rd);
    
    printf("Created thread for user %"PRIu8"\n",user->ID);

    // THIS is the per message read loop
    while(1)
    {
        // read a message
        read_bits(rd);

        // Generate the packet
        struct Packet* packet = generate_packet(rd->data);
	printf("Received packet from user %"PRIu8"\n",user->ID);

	// Print out the packet that was received
	print_packet_debug(packet->data,packet->dlength);
        // Lock the thread so no other threads will try to relay at the same time.
        //pthread_mutex_lock(&lock);
        // Relay the data to user.
        if (relay(packet) !=0){
            printf("Message relay error\n");
        }

        //Unlock the thread so that other threads can relay
        //pthread_mutex_unlock(&lock);
        
        //print_packet_debug(packet->data,packet->dlength);
	    free(packet->data);
	    free(packet);
        //reset readrate and run variables each iteration.
        reset_reader(rd);
        
    }

    //When done with the reading thread
    callback_cancel(id);

    // Free Data
    free(rd->data); //Do we need to free the data? pretty sure this is done in the read_to_file.
    free(rd);

    return NULL;
}

int main()
{
    pinit = pigpio_start(NULL, NULL);

    // Set GPIO modes
    set_mode(pinit, GPIO_SEND_1, PI_OUTPUT);
    set_mode(pinit, GPIO_RECEIVE_1, PI_INPUT);
    set_mode(pinit, GPIO_SEND_2, PI_OUTPUT);
    set_mode(pinit, GPIO_RECEIVE_2, PI_INPUT);
    set_mode(pinit, GPIO_SEND_3, PI_OUTPUT);
    set_mode(pinit, GPIO_RECEIVE_3, PI_INPUT);

    // Populate Addressbook of users
    //Allocate memory
    addressBook[0] = (struct User*) malloc(sizeof(struct User*));
    addressBook[1] = (struct User*) malloc(sizeof(struct User*));
    addressBook[2] = (struct User*) malloc(sizeof(struct User*));

    addressBook[0]->GPIO_OUT = GPIO_SEND_1;
    addressBook[0]->GPIO_IN = GPIO_RECEIVE_1;
    addressBook[0]->ID = 0x01;
    addressBook[0]->next = addressBook[1];

    addressBook[1]->GPIO_OUT = GPIO_SEND_2;
    addressBook[1]->GPIO_IN = GPIO_RECEIVE_2;
    addressBook[1]->ID = 0x02;
    addressBook[1]->next = NULL;
    
    addressBook[2]->GPIO_OUT = GPIO_SEND_3;
    addressBook[2]->GPIO_IN = GPIO_RECEIVE_3;
    addressBook[2]->ID = 0x03;
    addressBook[2]->next = NULL;
    // Create threads, one for each user

    // Initialize thread locking
    if (pthread_mutex_init(&lock, NULL) != 0) { 
        printf("\n mutex init has failed\n"); 
        return 1; 
    } 

    for (int i = 0; i< BOOKSIZE;i++) { 
	printf("i=%d\n",i);
        int error = pthread_create(&(tid[i]), NULL, &read_thread, addressBook[i]); 
        if (error != 0) 
            printf("\nThread can't be created : [%s]", strerror(error)); 
    } 

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    pthread_mutex_destroy(&lock); 

    pigpio_stop(pinit);

    return 0;
}
