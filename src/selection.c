#include "selection.h"
#include <stdio.h>
#include <stdlib.h>

const char* Application_List[NUM_APPS] = {"Chat", "Pong"};
const char* Address_Tenants[NUM_ADDRESSES] = {"Walt", "Dan", "Stuart"};

int select_application() {
    char selected_app_c[10];  // Fixed-size array
    printf("Select an application:\n");
    for (int i = 0; i < NUM_APPS; i++) {
        printf("[%d] %s\n", i, Application_List[i]);
    }
    // Get user input
    int selected_app = -1;
    while (selected_app < 0 || selected_app >= NUM_APPS) {
        printf("Enter the number corresponding to the application: ");
        fgets(selected_app_c, sizeof(selected_app_c), stdin);
        selected_app = atoi(selected_app_c);
        if (selected_app < 0 || selected_app >= NUM_APPS) {
            printf("Invalid selection. Please try again.\n");
        }
    }   

    printf("Selected Application: %s\n", Application_List[selected_app]);
    return selected_app;
}

uint8_t select_recipient() {
    char selected_address_c[10];  // Fixed-size array
    printf("Select a recipient: \n");
    for (int i = 0; i < NUM_ADDRESSES; i++) {
        printf("[%d] %s (ID: 0x%02X)\n", i, Address_Tenants[i], i + 1); // Assuming IDs start at 1
    }
    // Get user input
    int selected_address = -1;
    while (selected_address < 0 || selected_address >= NUM_ADDRESSES) {
        printf("Enter the number corresponding to the recipient: ");
        fgets(selected_address_c, sizeof(selected_address_c), stdin);
        selected_address = atoi(selected_address_c);
        if (selected_address < 0 || selected_address >= NUM_ADDRESSES) {
            printf("Invalid selection. Please try again.\n");
        }
    }

    uint8_t recipient_id = selected_address + 1; // Assuming recipient IDs start at 1
    printf("Selected Recipient: %s (ID: 0x%02X)\n", Address_Tenants[selected_address], recipient_id);

    return recipient_id;
}