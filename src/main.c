#include "message_app.h"
#include "build_packet.h"
#include "packet_defs.h"
#include "hamming.h"
#include "read.h"
#include "send.h"
#include "gpio_pairs.h"
#include "selection.h"
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>

#define DEVICE_TIMEOUT 90        // Timeout in seconds
#define MAINTENANCE_INTERVAL 30  // Check every 30 seconds
#define MY_ID 2                  // ID for this device
#define CONTROL_ADDRESS 0x00     // Reserved address for control packets
#define MAX_HOPS 16              // Maximum number of hops allowed
#define NUM_GPIO_PAIRS 4         // Define the number of GPIO pairs

// Routing Table Entry
typedef struct RoutingEntry {
    uint8_t destination_id;
    uint8_t next_hop;
    uint8_t metric;          
    time_t last_updated;    
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

// GPIO Pairs
struct GPIO_Pair gpio_pairs[NUM_GPIO_PAIRS] = {
    {26, 27, 0xFF},
    {24, 25, 0xFF},
    {22, 23, 0xFF},
    {20, 21, 0xFF}
};

int gpio_pins[NUM_GPIO_PAIRS] = {26, 24, 22, 20};  

// Global Variables
RoutingEntry* routingTable = NULL;
pthread_mutex_t routingTable_lock;
pthread_mutex_t gpio_mapping_lock;
pthread_mutex_t gpio_locks[NUM_GPIO_PAIRS];
int pinit;

// Function Declarations
RoutingEntry* find_routing_entry(uint8_t destination_id);
void update_routing_table(uint8_t source_id, uint8_t* data, size_t data_len);
void expire_routes();
void* routing_maintenance_thread(void* arg);
void* read_thread(void* arg);
void process_application_packet(struct Packet* packet);
void process_control_packet(struct Packet* packet, int gpio_in);
int relay(struct Packet* packet);
void* send_thread(void* arg);
void send_routing_update();
void print_routing_table();
int get_gpio_out_for_next_hop(uint8_t next_hop_id);
void remove_device_mapping(uint8_t device_id);
int gpio_out_to_index(int gpio_out);


void zero_all_lights(int pi) {
    for (int i = 0; i < NUM_GPIO_PAIRS; i++) {
        gpio_write(pi, gpio_pins[i], 0);  // Set each pin LOW (turn off)
    }
    printf("All lights have been set to 0.\n");
}

void print_routing_table() {
    pthread_mutex_lock(&routingTable_lock);
    RoutingEntry* current = routingTable;
    printf("Routing Table:\n");
    while (current != NULL) {
        printf("Destination ID: %" PRIu8 ", Next Hop: %" PRIu8 ", Metric: %" PRIu8 "\n",
               current->destination_id, current->next_hop, current->metric);
        current = current->next;
    }
    pthread_mutex_unlock(&routingTable_lock);
}

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

int gpio_out_to_index(int gpio_out) {
    for (int i = 0; i < NUM_GPIO_PAIRS; i++) {
        if (gpio_pairs[i].gpio_out == gpio_out) {
            return i;
        }
    }
    return -1;  // GPIO not found
}

// Send Routing Update
void send_routing_update() {
    pthread_mutex_lock(&routingTable_lock);

    uint8_t routing_data[256];
    size_t data_len = 0;

    RoutingEntry* current = routingTable;
    while (current != NULL && data_len + 2 <= sizeof(routing_data)) {
        if (current->metric >= MAX_HOPS) {
            current = current->next;
            continue;
        }

        routing_data[data_len++] = current->destination_id;
        routing_data[data_len++] = current->metric;

        current = current->next;
    }
    pthread_mutex_unlock(&routingTable_lock);

    if (data_len == 0) {
        printf("No routing entries to send in update. Skipping.\n");
        return;
    }

    // Build the packet
    uint8_t temp_pack[512];
    int packet_size = build_packet(MY_ID, CONTROL_ADDRESS, routing_data, data_len, temp_pack);

    pthread_mutex_lock(&gpio_mapping_lock);  // Lock the GPIO mapping
    for (int i = 0; i < NUM_GPIO_PAIRS; i++) {
        int gpio_out = gpio_pairs[i].gpio_out;
        
        // Lock the mutex for the gpio_out
        int gpio_index = gpio_out_to_index(gpio_out);
        pthread_mutex_lock(&gpio_locks[gpio_index]);  // Ensure exclusive access

        if (send_bytes(temp_pack, packet_size, gpio_out, pinit) != 0) {
            fprintf(stderr, "Failed to send routing update on GPIO %d\n", gpio_out);
        } else {
            printf("Routing update sent on GPIO %d\n", gpio_out);
        }

        // Unlock the mutex after sending
        pthread_mutex_unlock(&gpio_locks[gpio_index]);
    }
    pthread_mutex_unlock(&gpio_mapping_lock);  // Unlock GPIO mapping
}

// Expire stale routes
void expire_routes() {
    pthread_mutex_lock(&routingTable_lock);
    time_t current_time = time(NULL);
    RoutingEntry** current = &routingTable;
    while (*current != NULL) {
        if (current_time - (*current)->last_updated > DEVICE_TIMEOUT) {
            printf("Route to ID %" PRIu8 " has expired.\n", (*current)->destination_id);
            RoutingEntry* expired = *current;
            *current = (*current)->next;
            free(expired);
        } else {
            current = &(*current)->next;
        }
    }
    pthread_mutex_unlock(&routingTable_lock);
}

// Routing Maintenance Thread
void* routing_maintenance_thread(void* arg) {
    while (1) {
        sleep(MAINTENANCE_INTERVAL);

        expire_routes();

        // Periodically send routing updates
        printf("Sending periodic routing update.\n");
        send_routing_update();
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

        if (packet->sending_addy == MY_ID) {
            printf("Received packet sent by ourselves. Discarding.\n");
            free(packet->data);
            free(packet);
            reset_reader(rd);
            continue;
        }

        if (packet->receiving_addy == MY_ID || packet->receiving_addy == CONTROL_ADDRESS) {
            if (packet->receiving_addy == CONTROL_ADDRESS) {
                process_control_packet(packet, gpio_in);
            } else {
                process_application_packet(packet);
            }
        } else {
            printf("Relaying packet\n");
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

// Process Application Packet
void process_application_packet(struct Packet* packet) {
    // Delegate message decoding to message_app
    read_message(packet->data, packet->dlength);
}

// Process Control Packet
void process_control_packet(struct Packet* packet, int gpio_in) {
    // Update the routing table
    update_routing_table(packet->sending_addy, packet->data, packet->dlength);

    // Map the sending device ID to the GPIO input port
    pthread_mutex_lock(&gpio_mapping_lock);
    for (int i = 0; i < NUM_GPIO_PAIRS; i++) {
        if (gpio_pairs[i].gpio_in == gpio_in) {
            gpio_pairs[i].connected_device_id = packet->sending_addy;
            printf("Mapped device ID %" PRIu8 " to GPIO_IN %d and GPIO_OUT %d\n",
                   packet->sending_addy, gpio_pairs[i].gpio_in, gpio_pairs[i].gpio_out);
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
            printf("Next hop %" PRIu8 " found at GPIO_OUT %d\n", next_hop_id, gpio_out);
            pthread_mutex_unlock(&gpio_mapping_lock);
            return gpio_out;
        }
    }
    pthread_mutex_unlock(&gpio_mapping_lock);
    printf("Next hop %" PRIu8 " not found in GPIO mappings\n", next_hop_id);
    return -1;  // Not found
}

int relay(struct Packet* packet) {
    pthread_mutex_lock(&routingTable_lock);
    RoutingEntry* entry = find_routing_entry(packet->receiving_addy);

    if (entry == NULL || entry->metric >= MAX_HOPS) {
        pthread_mutex_unlock(&routingTable_lock);
        print_routing_table();
        fprintf(stderr, "No route to destination ID %" PRIu8 "\n", packet->receiving_addy);
        return 1;
    }

    // Determine the GPIO output port based on the next hop
    int gpio_out = get_gpio_out_for_next_hop(entry->next_hop);
    pthread_mutex_unlock(&routingTable_lock);
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
            encoded_payload = send_message(&data_size, &recipient_id);
        } else if (app_data->selected_application == 1) {
            continue; // Skip pong for now
        } else {
            return NULL;
        }

        if (!encoded_payload || data_size == 0) {
            fprintf(stderr, "Failed to get encoded message.\n");
            continue;
        }

        app_data->selected_recipient = recipient_id;

        pthread_mutex_lock(&routingTable_lock);
        RoutingEntry* entry = find_routing_entry(recipient_id);
        pthread_mutex_unlock(&routingTable_lock);

        if (entry == NULL || entry->metric >= MAX_HOPS) {
            fprintf(stderr, "No route to destination ID %" PRIu8 "\n", recipient_id);
            free(encoded_payload);
            continue;
        }

        int gpio_out = get_gpio_out_for_next_hop(entry->next_hop);
        if (gpio_out == -1) {
            fprintf(stderr, "No GPIO output port found for next hop %" PRIu8 "\n", entry->next_hop);
            free(encoded_payload);
            continue;
        }

        // Lock the mutex for the gpio_out
        int gpio_index = gpio_out_to_index(gpio_out); 
        pthread_mutex_lock(&gpio_locks[gpio_index]);

        // Build and send the packet
        uint8_t temp_pack[512];
        int packet_size = build_packet(MY_ID, recipient_id, encoded_payload, data_size, temp_pack);

        if (packet_size < 0) {
            fprintf(stderr, "Failed to build packet.\n");
            pthread_mutex_unlock(&gpio_locks[gpio_index]);
            continue;
        }

        if (send_bytes(temp_pack, packet_size, gpio_out, app_data->pinit) != 0) {
            fprintf(stderr, "Failed to send message on GPIO %d\n", gpio_out);
        } else {
            printf("Message sent successfully to ID %" PRIu8 " on GPIO %d\n", recipient_id, gpio_out);
        }

        // Unlock the mutex after sending
        pthread_mutex_unlock(&gpio_locks[gpio_index]);

        free(encoded_payload);
    }

    //free(encoded_payload);
    //free(temp_pack);
    return NULL;
}


int main() {

    struct AppData app_data;

    // Application selection
    app_data.selected_application = select_application();

    // Initialize pigpio
    app_data.pinit = pigpio_start(NULL, NULL);

    zero_all_lights(app_data.pinit);
    
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

    for (int i = 0; i < NUM_GPIO_PAIRS; i++) {
    if (pthread_mutex_init(&gpio_locks[i], NULL) != 0) {
        fprintf(stderr, "Failed to initialize mutex for GPIO pair %d\n", i);
        return 1;
    }
}

    // Initialize routing table with self entry
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
    selfEntry->metric = 0; // Metric to self is 0
    selfEntry->last_updated = time(NULL); // Initialize last_updated
    selfEntry->next = NULL;

    pthread_mutex_lock(&routingTable_lock);
    routingTable = selfEntry;
    pthread_mutex_unlock(&routingTable_lock);
    send_routing_update();
    // Start threads
    pthread_t read_tids[NUM_GPIO_PAIRS], send_tid, maintenance_tid;

    // Create read threads for each GPIO input
    for (int i = 0; i < NUM_GPIO_PAIRS; i++) {
        struct ReadThreadData* rt_data = malloc(sizeof(struct ReadThreadData));
        if (!rt_data) {
            fprintf(stderr, "Memory allocation failed for ReadThreadData\n");
            // Handle cleanup and exit
            pigpio_stop(app_data.pinit);
            return 1;
        }
        rt_data->pinit = app_data.pinit;
        rt_data->gpio_in = gpio_pairs[i].gpio_in;

        if (pthread_create(&read_tids[i], NULL, read_thread, rt_data) != 0) {
            fprintf(stderr, "Failed to create read thread for GPIO %d\n", gpio_pairs[i].gpio_in);
            free(rt_data);
            // Handle cleanup and exit
            pigpio_stop(app_data.pinit);
            return 1;
        }
    }

    // Create send thread
    if (pthread_create(&send_tid, NULL, send_thread, &app_data) != 0) {
        fprintf(stderr, "Failed to create send thread\n");
        // Handle cleanup and exit
        pigpio_stop(app_data.pinit);
        return 1;
    }

    // Create routing maintenance thread
    if (pthread_create(&maintenance_tid, NULL, routing_maintenance_thread, NULL) != 0) {
        fprintf(stderr, "Failed to create routing maintenance thread\n");
        // Handle cleanup and exit
        pigpio_stop(app_data.pinit);
        return 1;
    }

    // Join threads
    for (int i = 0; i < NUM_GPIO_PAIRS; i++) {
        pthread_join(read_tids[i], NULL);
    }
    pthread_join(send_tid, NULL);
    pthread_join(maintenance_tid, NULL);

    // Destroy mutexes
    pthread_mutex_destroy(&routingTable_lock);
    pthread_mutex_destroy(&gpio_mapping_lock);

    for (int i = 0; i < NUM_GPIO_PAIRS; i++) {
    pthread_mutex_destroy(&gpio_locks[i]);
    }
    // Stop pigpio
    pigpio_stop(app_data.pinit);

    return 0;
}
