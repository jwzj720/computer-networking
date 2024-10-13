#include "selection.h"

const char* Application_List[NUM_APPS] = {"Chat", "Pong"};
const char* Address_Book[NUM_ADDRESSES] = {"0x01", "0x02", "0x03"};
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
        fflush(stdin);
        selected_app = selected_app_c[0] - '0';
        if (selected_app < 0 || selected_app >= NUM_APPS) {
            printf("Invalid selection. Please try again.\n");
        }
    }   

    printf("Selected Application: %s\n", Application_List[selected_app]);
    return selected_app;
}

int select_address(const char** rec_name) {
    char selected_address_c[10];  // Fixed-size array
    printf("Select a recipient: \n");
    for (int i = 0; i < NUM_ADDRESSES; i++) {
        printf("[%d] %s\n", i, Address_Tenants[i]);
    }
    // Get user input
    int selected_address = -1;
    while (selected_address < 0 || selected_address >= NUM_ADDRESSES) {
        printf("Enter the number corresponding to the address: ");
        fgets(selected_address_c, sizeof(selected_address_c), stdin);
        fflush(stdin);
        selected_address = selected_address_c[0] - '0';
        if (selected_address < 0 || selected_address >= NUM_ADDRESSES) {
            printf("Invalid selection. Please try again.\n");
        }
    }

    *rec_name = Address_Tenants[selected_address];

    return (uint8_t)selected_address;
}