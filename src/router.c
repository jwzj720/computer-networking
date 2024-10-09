//struct User* addressBook[BOOKSIZE]; /* pointer table */
struct User* addressBook = NULL;

//pthread_t tid[BOOKSIZE];
//pthread_t write_thread;

//pthread_mutex_t lock;

pthread_mutex_t addressBook_lock;  /* lock for synchronizing access to the address book */
pthread_mutex_t gpio_out_lock;     /* lock for synchronizing access to GPIO_OUT_PINS */
int pinit;



int GPIO_IN_PINS[MAX_GPIO_PINS] = GPIO_RECEIVE_PINS;
int GPIO_OUT_PINS[MAX_GPIO_PINS] = GPIO_SEND_PINS;


/* HELPER FUNCTIONS */

// Right now lookup acts like a linked list, goes through every value
// Could be implemented to act like hash table with faster lookup
struct User* lookup(uint8_t id) {
    struct User* current = addressBook;
    while (current != NULL) {
        if (current->ID == id) {
            return current;  /* User found */
        }
        current = current->next;
    }
    return NULL;  
}

int get_gpio_out(int gpio_in) {
    for (int i = 0; i < NUM_GPIO_PAIRS; i++) {
        if (gpio_pairs[i].gpio_in == gpio_in) {
            return gpio_pairs[i].gpio_out;
        }
    }
    return -1; /* Not found */
}

void* read_thread(void* arg) {
    
    int gpio_in = *((int*) arg);
    free(arg);

    struct ReadData* rd = create_reader(ROUTER_ADDRESS);

    /* Register the callback */
    int callback_id = callback_ex(pinit, gpio_in, EITHER_EDGE, get_bit, rd);
    

    while (1) {
        /* Read a message */
        read_bits(rd);

        /* Generate the packet */

        struct Packet* packet = generate_packet(rd->data);
        print_packet_debug(packet->data,packet->dlength);
        /* Process the packet based on receiving address */
        if (packet->receiving_addy == ROUTER_ADDRESS) {
            process_control_packet(packet, gpio_in);
        } else {
            process_application_packet(packet);
        }

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

void process_application_packet(struct Packet* packet) {
    if (relay(packet) != 0) {
        fprintf(stderr, "Message relay error\n");
    }
}

void process_control_packet(struct Packet* packet, int gpio_in) {
    pthread_mutex_lock(&addressBook_lock);
    struct User* sender = lookup(packet->sending_addy);

    if (sender == NULL) {
        /* New device -> add to address book */
        struct User* newUser = malloc(sizeof(struct User));
        newUser->ID = packet->sending_addy;
        newUser->GPIO_IN = gpio_in;

        /* Get corresponding GPIO_OUT pin */
        int gpio_out = get_gpio_out(gpio_in);
        if (gpio_out == -1) {
            fprintf(stderr, "No matching GPIO_OUT pin for GPIO_IN pin %d\n", gpio_in);
            free(newUser);
        } else {
            newUser->GPIO_OUT = gpio_out;

            /* Add new user to the address book */
            newUser->next = addressBook;
            addressBook = newUser;
            printf("Added new User with ID %" PRIu8 ", GPIO_IN: %d, GPIO_OUT: %d\n", newUser->ID, newUser->GPIO_IN, newUser->GPIO_OUT);
        }
    }
    pthread_mutex_unlock(&addressBook_lock);
}

int relay(struct Packet* pack) {
    // find the desination
    pthread_mutex_lock(&addressBook_lock);
    struct User* dest = lookup(pack->receiving_addy);
    pthread_mutex_unlock(&addressBook_lock);

    if (dest == NULL) {
        fprintf(stderr, "Intended recipient not known by router.\n");
        return 1;
    }

    uint8_t temp_pack[50];
    // build the packet
    int packet_size = build_packet(pack->sending_addy, pack->receiving_addy, pack->data, pack->dlength, temp_pack);

    // send the packet
    if (send_bytes(temp_pack, packet_size, dest->GPIO_OUT, pinit) != 0) {
        fprintf(stderr, "Failed to send packet\n");
        return 1;
    }
    return 0;
}



int main() {
    pinit = pigpio_start(NULL, NULL);

    // Initialize GPIO 
    for (int i = 0; i < NUM_GPIO_PAIRS; i++) {
        set_mode(pinit, gpio_pairs[i].gpio_in, PI_INPUT);
        set_mode(pinit, gpio_pairs[i].gpio_out, PI_OUTPUT);
    }

    // Initialize locks 
    if (pthread_mutex_init(&addressBook_lock, NULL) != 0) {
        fprintf(stderr, "Mutex initialization failed\n");
        return 1;
    }

    // Create reading threads for each GPIO_IN pin 
    pthread_t tid[NUM_GPIO_PAIRS];
    for (int i = 0; i < NUM_GPIO_PAIRS; i++) {
        int* gpio_in_ptr = malloc(sizeof(int));
        *gpio_in_ptr = gpio_pairs[i].gpio_in;
        if (pthread_create(&tid[i], NULL, &read_thread, gpio_in_ptr) != 0) {
            fprintf(stderr, "Thread creation failed for GPIO_IN pin %d\n", gpio_pairs[i].gpio_in);
            free(gpio_in_ptr);
        }
    }

    // Wait for threads to finish 
    for (int i = 0; i < NUM_GPIO_PAIRS; i++) {
        pthread_join(tid[i], NULL);
    }

    pthread_mutex_destroy(&addressBook_lock);
    pigpio_stop(pinit);

    return 0;
}