#include "message_app.h"
#include "build_packet.h"
#include "packet_defs.h"
#include "hamming.h"
#include "read.h"
#include "send.h"
#include "gpio_pairs.h"
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

struct ReadThreadData {
    int pinit;
    int gpio_in;
};

struct GPIO_Pair gpio_pairs[NUM_GPIO_PAIRS] = {
    {26, 27, 0}, 
    {24, 25, 0},
    {22, 23, 0},
    {20, 21, 0}
};

// Global Variables
RoutingEntry* routingTable = NULL;
pthread_mutex_t routingTable_lock;
pthread_mutex_t gpio_mapping_lock;

int pinit;

// Function Declarations
RoutingEntry* find_routing_entry(uint8_t destination_id);
void update_routing_table(uint8_t source_id, uint8_t* data, size_t data_len);
void* routing_update_thread(void* arg);
void* read_thread(void* arg);
void process_application_packet(struct Packet* packet);
void process_control_packet(struct Packet* packet, int gpio_in);
int relay(struct Packet* packet);
void* send_thread(void* arg);


// Find a routing table entry for a given client 
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
            // Add new routing entry 
            RoutingEntry* newEntry = malloc(sizeof(RoutingEntry));
            newEntry->destination_id = dest_id;
            newEntry->next_hop = source_id;
            newEntry->hop_count = new_hop_count;
            newEntry->next = routingTable;
            routingTable = newEntry;
            printf("Added route to ID: %" PRIu8 ", Next Hop: %" PRIu8 ", Hops: %" PRIu8 "\n", dest_id, source_id, new_hop_count);
        } else {
            // Update existing entry if new path is better 
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

        // Send routing update through all GPIO output ports
        for (int i = 0; i < NUM_GPIO_PAIRS; i++) {
            int gpio_out = gpio_pairs[i].gpio_out;
            if (send_bytes(temp_pack, packet_size, gpio_out, pinit) != 0) {
                fprintf(stderr, "Failed to send routing update on GPIO %d\n", gpio_out);
            } else {
                printf("Routing update sent on GPIO %d\n", gpio_out);
            }
        }

        time_sleep(UPDATE_INTERVAL);
    }
    return NULL;
}

// Read Thread 
void* read_thread(void* arg) {
    struct ReadThreadData* data = (struct ReadThreadData*)arg;
    int pinit = data->pinit;
    int gpio_in = data->gpio_in;

    struct ReadData* rd = create_reader(MY_ID);

    int callback_id = callback_ex(pinit, gpio_in, EITHER_EDGE, get_bit, rd);

    while (1) {
        read_bits(rd);

        struct Packet* packet = generate_packet(rd->data);
        print_packet_debug(packet->data, packet->dlength);

        if (packet->receiving_addy == MY_ID) {
            process_application_packet(packet);
        } else if (packet->receiving_addy == CONTROL_ADDRESS) {
            process_control_packet(packet, gpio_in); // Added gpio_in
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
    free(data);

    return NULL;
}

void process_application_packet(struct Packet* packet) {
    // Delegate message decoding to message_app
    read_message(packet->data, packet->dlength);
}
// Process Control Packet
void process_control_packet(struct Packet* packet, int gpio_in) {
    // Update routing table
    update_routing_table(packet->sending_addy, packet->data, packet->dlength);

    // Map the sending device ID to the GPIO input port
    pthread_mutex_lock(&gpio_mapping_lock);
    for (int i = 0; i < NUM_GPIO_PAIRS; i++) {
        if (gpio_pairs[i].gpio_in == gpio_in) {
            gpio_pairs[i].connected_device_id = packet->sending_addy;
            printf("Mapped device ID %" PRIu8 " to GPIO_IN %d and GPIO_OUT %d\n", packet->sending_addy, gpio_pairs[i].gpio_in, gpio_pairs[i].gpio_out);
            break;
        }
    }
    pthread_mutex_unlock(&gpio_mapping_lock);
}

// Relay Packet 
int get_gpio_out_for_next_hop(uint8_t next_hop_id) {
    pthread_mutex_lock(&gpio_mapping_lock);
    for (int i = 0; i < NUM_GPIO_PAIRS; i++) {
        if (gpio_pairs[i].connected_device_id == next_hop_id) {
            int gpio_out = gpio_pairs[i].gpio_out;
            pthread_mutex_unlock(&gpio_mapping_lock);
            return gpio_out;
        }
    }
    pthread_mutex_unlock(&gpio_mapping_lock);
    return -1; // Not found
}

int relay(struct Packet* packet) {
    pthread_mutex_lock(&routingTable_lock);
    RoutingEntry* entry = find_routing_entry(packet->receiving_addy);
    pthread_mutex_unlock(&routingTable_lock);

    if (entry == NULL) {
        fprintf(stderr, "No route to destination ID %" PRIu8 "\n", packet->receiving_addy);
        return 1;
    }

    // Determine the GPIO output port based on the next hop
    int gpio_out = get_gpio_out_for_next_hop(entry->next_hop);
    if (gpio_out == -1) {
        fprintf(stderr, "No GPIO output port found for next hop %" PRIu8 "\n", entry->next_hop);
        return 1;
    }

    // Build the packet
    uint8_t temp_pack[512];
    int packet_size = build_packet(packet->sending_addy, packet->receiving_addy, packet->data, packet->dlength, temp_pack);

    // Send the packet to the next hop
    if (send_bytes(temp_pack, packet_size, gpio_out, pinit) != 0) {
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
        uint8_t recipient_id;
        uint8_t* encoded_payload = NULL;

        if (app_data->selected_application == 0) {
            // Call send_message which now handles recipient selection
            encoded_payload = send_message(&data_size, &recipient_id);
        } else if (app_data->selected_application == 1) {
            // Pong application logic...
            continue; // For now, skip until Stuart has logic for pong.
        } else {
            return NULL;
        }

        if (!encoded_payload || data_size == 0) {
            fprintf(stderr, "Failed to get encoded message.\n");
            continue;
        }

        // Store the recipient ID in app_data
        app_data->selected_recipient = recipient_id;

        pthread_mutex_lock(&routingTable_lock);
        RoutingEntry* entry = find_routing_entry(recipient_id);
        pthread_mutex_unlock(&routingTable_lock);

        if (entry == NULL) {
            fprintf(stderr, "No route to destination ID %" PRIu8 "\n", recipient_id);
            free(encoded_payload);
            continue;
        }

        // Determine the GPIO output port based on the next hop
        int gpio_out = get_gpio_out_for_next_hop(entry->next_hop);
        if (gpio_out == -1) {
            fprintf(stderr, "No GPIO output port found for next hop %" PRIu8 "\n", entry->next_hop);
            free(encoded_payload);
            continue;
        }

        // Build the packet
        uint8_t temp_pack[MAX_PACKET_SIZE];
        int packet_size = build_packet(MY_ID, recipient_id, encoded_payload, data_size, temp_pack);
        free(encoded_payload); // Free encoded payload

        if (packet_size < 0) {
            fprintf(stderr, "Failed to build packet.\n");
            continue;
        }

        // Send the packet to the next hop
        if (send_bytes(temp_pack, packet_size, gpio_out, app_data->pinit) != 0) {
            fprintf(stderr, "Failed to send message\n");
            continue;
        }

        printf("Message sent successfully to ID %" PRIu8 "\n", recipient_id);
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

    // Set GPIO modes for all GPIO pairs
    for (int i = 0; i < NUM_GPIO_PAIRS; i++) {
        if (set_mode(app_data.pinit, gpio_pairs[i].gpio_out, PI_OUTPUT) != 0 ||
            set_mode(app_data.pinit, gpio_pairs[i].gpio_in, PI_INPUT) != 0) {
            pigpio_stop(app_data.pinit);
            return 1;
        }
    }

    // Initialize mutexes
    if (pthread_mutex_init(&routingTable_lock, NULL) != 0) {
        fprintf(stderr, "Mutex initialization failed\n");
        pigpio_stop(app_data.pinit);
        return 1;
    }

    if (pthread_mutex_init(&gpio_mapping_lock, NULL) != 0) {
        fprintf(stderr, "Mutex initialization failed\n");
        pthread_mutex_destroy(&routingTable_lock);
        pigpio_stop(app_data.pinit);
        return 1;
    }

    // Initialize routing table
    RoutingEntry* selfEntry = malloc(sizeof(RoutingEntry));
    if (!selfEntry) {
        fprintf(stderr, "Memory allocation failed for routing table\n");
        pthread_mutex_destroy(&routingTable_lock);
        pthread_mutex_destroy(&gpio_mapping_lock);
        pigpio_stop(app_data.pinit);
        return 1;
    }
    selfEntry->destination_id = MY_ID;
    selfEntry->next_hop = MY_ID;
    selfEntry->hop_count = 0;
    selfEntry->next = NULL;

    pthread_mutex_lock(&routingTable_lock);
    routingTable = selfEntry;
    pthread_mutex_unlock(&routingTable_lock);

    // Start threads
    pthread_t read_tids[NUM_GPIO_PAIRS], send_tid, routing_update_tid;

    // Create read threads for each GPIO input
    for (int i = 0; i < NUM_GPIO_PAIRS; i++) {
        struct ReadThreadData* rt_data = malloc(sizeof(struct ReadThreadData));
        if (!rt_data) {
            fprintf(stderr, "Memory allocation failed for ReadThreadData\n");
            // Handle cleanup and exit
        }
        rt_data->pinit = app_data.pinit;
        rt_data->gpio_in = gpio_pairs[i].gpio_in;

        if (pthread_create(&read_tids[i], NULL, read_thread, rt_data) != 0) {
            fprintf(stderr, "Failed to create read thread for GPIO %d\n", gpio_pairs[i].gpio_in);
            free(rt_data);
            // Handle cleanup and exit
        }
    }

    // Create send thread
    if (pthread_create(&send_tid, NULL, send_thread, &app_data) != 0) {
        fprintf(stderr, "Failed to create send thread\n");
        // Handle cleanup and exit
    }

    // Create routing update thread
    if (pthread_create(&routing_update_tid, NULL, routing_update_thread, &app_data.pinit) != 0) {
        fprintf(stderr, "Failed to create routing update thread\n");
        // Handle cleanup and exit
    }

    // Join threads
    for (int i = 0; i < NUM_GPIO_PAIRS; i++) {
        pthread_join(read_tids[i], NULL);
    }
    pthread_join(send_tid, NULL);
    pthread_join(routing_update_tid, NULL);

    // Destroy mutexes
    pthread_mutex_destroy(&routingTable_lock);
    pthread_mutex_destroy(&gpio_mapping_lock);

    // Stop pigpio
    pigpio_stop(app_data.pinit);

    return 0;
}
