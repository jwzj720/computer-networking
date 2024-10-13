#include "message_app.h"
#include "build_packet.h"
#include "packet_defs.h"
#include "hamming.h"
#include "read.h"
#include "send.h"
#include "selection.h"
#include "router.h"
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>


#define MY_ID 1              // ID for this device
#define CONTROL_ADDRESS 0x00 // Reserved address for control packets
#define MAX_HOPS 16          // Maximum number of hops allowed
#define UPDATE_INTERVAL 10   // Interval for sending routing updates in seconds


// Routing Table Entry
typedef struct RoutingEntry {
    uint8_t destination_id;
    uint8_t next_hop;
    uint8_t hop_count;
    struct RoutingEntry* next;
} RoutingEntry;

// Application Data 
struct AppData {
    int pinit;
    int selected_application;
    uint8_t selected_recipient;
};

// Global Variables
RoutingEntry* routingTable = NULL;
pthread_mutex_t routingTable_lock;
int pinit;

// Function Declarations
RoutingEntry* find_routing_entry(uint8_t destination_id);
void update_routing_table(uint8_t source_id, uint8_t* data, size_t data_len);
void* routing_update_thread(void* arg);
void* read_thread(void* arg);
void process_application_packet(struct Packet* packet);
void process_control_packet(struct Packet* packet);
int relay(struct Packet* packet);
void* send_thread(void* arg);


// Find a routing table entry for a given destination 
RoutingEntry* find_routing_entry(uint8_t destination_id) {
    RoutingEntry* current = routingTable;
    while (current != NULL) {
        if (current->destination_id == destination_id) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Update the routing table based on received routing information 
void update_routing_table(uint8_t source_id, uint8_t* data, size_t data_len) {
    pthread_mutex_lock(&routingTable_lock);

    size_t i = 0;
    while (i + 2 <= data_len) {
        uint8_t dest_id = data[i];
        uint8_t hop_count = data[i + 1];

        if (dest_id == MY_ID) {
            i += 2;
            continue;
        }

        RoutingEntry* entry = find_routing_entry(dest_id);
        uint8_t new_hop_count = hop_count + 1;

        if (new_hop_count > MAX_HOPS) {
            i += 2;
            continue;
        }

        if (entry == NULL) {
            /* Add new routing entry */
            RoutingEntry* newEntry = malloc(sizeof(RoutingEntry));
            newEntry->destination_id = dest_id;
            newEntry->next_hop = source_id;
            newEntry->hop_count = new_hop_count;
            newEntry->next = routingTable;
            routingTable = newEntry;
            printf("Added route to ID: %" PRIu8 ", Next Hop: %" PRIu8 ", Hops: %" PRIu8 "\n", dest_id, source_id, new_hop_count);
        } else {
            /* Update existing entry if new path is better */
            if (new_hop_count < entry->hop_count) {
                entry->next_hop = source_id;
                entry->hop_count = new_hop_count;
                printf("Updated route to ID: %" PRIu8 ", Next Hop: %" PRIu8 ", Hops: %" PRIu8 "\n", dest_id, source_id, new_hop_count);
            }
        }
        i += 2;
    }

    pthread_mutex_unlock(&routingTable_lock);
}

// Routing Update Thread 
void* routing_update_thread(void* arg) {
    int pinit = *(int*)arg;
    while (1) {
        // Build routing message 
        pthread_mutex_lock(&routingTable_lock);
        uint8_t routing_data[256];
        size_t data_len = 0;

        RoutingEntry* current = routingTable;
        while (current != NULL && data_len + 2 <= sizeof(routing_data)) {
            routing_data[data_len++] = current->destination_id;
            routing_data[data_len++] = current->hop_count;
            current = current->next;
        }
        pthread_mutex_unlock(&routingTable_lock);

        // Build the packet 
        uint8_t temp_pack[512];
        int packet_size = build_packet(MY_ID, CONTROL_ADDRESS, routing_data, data_len, temp_pack);

        // Send routing update
        if (send_bytes(temp_pack, packet_size, GPIO_SEND, pinit) != 0) {
            fprintf(stderr, "Failed to send routing update\n");
        } else {
            printf("Routing update sent\n");
        }

        time_sleep(UPDATE_INTERVAL);
    }
    return NULL;
}

// Read Thread 
void* read_thread(void* arg) {
    int pinit = *(int*)arg;

    struct ReadData* rd = create_reader(MY_ID);

    int callback_id = callback_ex(pinit, GPIO_RECEIVE, EITHER_EDGE, get_bit, rd);

    while (1) {
        read_bits(rd);

        struct Packet* packet = generate_packet(rd->data);
        print_packet_debug(packet->data, packet->dlength);

        if (packet->receiving_addy == MY_ID) {
            process_application_packet(packet);
        } else if (packet->receiving_addy == CONTROL_ADDRESS) {
            process_control_packet(packet);
        } else {
            relay(packet);
        }

        free(packet->data);
        free(packet);
        reset_reader(rd);
    }

    callback_cancel(callback_id);
    free(rd->data);
    free(rd);

    return NULL;
}

// Process Application Packet 
void process_application_packet(struct Packet* packet) {
    size_t decoded_len;
    read_message(packet->data, packet->dlength, &decoded_len);
}

// Process Control Packet
void process_control_packet(struct Packet* packet) {
    //Update routing table 
    update_routing_table(packet->sending_addy, packet->data, packet->dlength);
}

// Relay Packet 
int relay(struct Packet* packet) {
    pthread_mutex_lock(&routingTable_lock);
    RoutingEntry* entry = find_routing_entry(packet->receiving_addy);
    pthread_mutex_unlock(&routingTable_lock);

    if (entry == NULL) {
        fprintf(stderr, "No route to destination ID %" PRIu8 "\n", packet->receiving_addy);
        return 1;
    }

    // Build the packet 
    uint8_t temp_pack[512];
    int packet_size = build_packet(packet->sending_addy, packet->receiving_addy, packet->data, packet->dlength, temp_pack);

    // Send the packet to the next hop 
    if (send_bytes(temp_pack, packet_size, GPIO_SEND, pinit) != 0) {
        fprintf(stderr, "Failed to relay packet\n");
        return 1;
    }
    return 0;
}

// Send Thread 
void* send_thread(void* arg) {
    struct AppData* app_data = (struct AppData*)arg;

    while (1) {
        size_t data_size;
        uint8_t* payload = NULL;

        if (app_data->selected_application == 0) {
            payload = send_message(&data_size);
        } else if (app_data->selected_application == 1) {
            /* Pong application logic here */
        } else {
            return NULL;
        }

        /* For testing purposes, let's select the recipient each time */
        app_data->selected_recipient = select_recipient();

        pthread_mutex_lock(&routingTable_lock);
        RoutingEntry* entry = find_routing_entry(app_data->selected_recipient);
        pthread_mutex_unlock(&routingTable_lock);

        if (entry == NULL) {
            fprintf(stderr, "No route to destination ID %" PRIu8 "\n", app_data->selected_recipient);
            free(payload);
            continue;
        }

        // Build the packet 
        uint8_t temp_pack[512];
        int packet_size = build_packet(MY_ID, app_data->selected_recipient, payload, data_size, temp_pack);

        // Send the packet to the next hop 
        if (send_bytes(temp_pack, packet_size, GPIO_SEND, app_data->pinit) != 0) {
            fprintf(stderr, "Failed to send message\n");
            free(payload);
            continue;
        }

        free(payload);
    }

    return NULL;
}

int main() {
    struct AppData app_data;

    // Application selection 
    app_data.selected_application = select_application();

    // Initialize pigpio 
    app_data.pinit = pigpio_start(NULL, NULL);
    pinit = app_data.pinit;

    if (app_data.pinit < 0) {
        fprintf(stderr, "Failed to initialize pigpio\n");
        return 1;
    }

    // Set GPIO modes 
    if (set_mode(app_data.pinit, GPIO_SEND, PI_OUTPUT) != 0 ||
        set_mode(app_data.pinit, GPIO_RECEIVE, PI_INPUT) != 0) {
        pigpio_stop(app_data.pinit);
        return 1;
    }

    // locks
    if (pthread_mutex_init(&routingTable_lock, NULL) != 0) {
        fprintf(stderr, "Mutex initialization failed\n");
        return 1;
    }

    // routing table
    RoutingEntry* selfEntry = malloc(sizeof(RoutingEntry));
    selfEntry->destination_id = MY_ID;
    selfEntry->next_hop = MY_ID;
    selfEntry->hop_count = 0;
    selfEntry->next = NULL;

    pthread_mutex_lock(&routingTable_lock);
    routingTable = selfEntry;
    pthread_mutex_unlock(&routingTable_lock);

    // start threads
    pthread_t read_tid, send_tid, routing_update_tid;

    if (pthread_create(&read_tid, NULL, read_thread, &app_data.pinit) != 0) {
        fprintf(stderr, "Failed to create read thread\n");
        return 1;
    }

    if (pthread_create(&send_tid, NULL, send_thread, &app_data) != 0) {
        fprintf(stderr, "Failed to create send thread\n");
        return 1;
    }

    if (pthread_create(&routing_update_tid, NULL, routing_update_thread, &app_data.pinit) != 0) {
        fprintf(stderr, "Failed to create routing update thread\n");
        return 1;
    }

    // join threads
    pthread_join(read_tid, NULL);
    pthread_join(send_tid, NULL);
    pthread_join(routing_update_tid, NULL);

    // stop pigpio and locks
    pthread_mutex_destroy(&routingTable_lock);
    pigpio_stop(app_data.pinit);

    return 0;
}
