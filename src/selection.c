// utility class for selection functions

#include "selection.h"

const char* Application_List[NUM_APPS] = {"Chat", "Pong"};
const char* Address_Book[NUM_ADDRESSES] = {"0x01", "0x02", "0x03"};
const char* Address_Tenents[NUM_ADDRESSES] = {"Walt", "Dan", "Stuart"};

int select_application(){
    int selected_app = -1;

    printf("Select an application:\\n");
    for (int i = 0; i < NUM_APPS; i++) {
        printf("[%d] %s\n", i, Application_List[i]);
    }

    // Get user input
    while (selected_app < 0 || selected_app >= NUM_APPS) {
        printf("Enter the number corresponding to the application: ");
        scanf("%d", &selected_app);

        if (selected_app < 0 || selected_app >= NUM_APPS) {
            printf("Invalid selection. Please try again.\n");
        }
    }   

printf("Selected Application: %s\n", Application_List[selected_app]);
return selected_app;
}

int select_address(){
    int selected_address = -1;

    printf("Select a recipient: \n");
    for (int i = 0; i < NUM_ADDRESSES; i++) {
        printf("[%d] %s\n", i, Address_Tenents[i]);
    }
    // Get user input
    while (selected_address < 0 || selected_address >= NUM_ADDRESSES) {
        printf("Enter the number corresponding to the address: ");
        scanf("%d", &selected_address);

        if (selected_address < 0 || selected_address >= NUM_ADDRESSES) {
            printf("Invalid selection. Please try again.\n");
        }
    }

    printf("Selected Address: %s\n", Address_Book[selected_address]);
    return selected_address;
}