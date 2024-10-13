// utility class for selection functions

#include "selection.h"

const char* Application_List[NUM_APPS] = {"Chat", "Pong"};
const char* Address_Book[NUM_ADDRESSES] = {"0x01", "0x02", "0x03"};
const char* Address_Tenents[NUM_ADDRESSES] = {"Walt", "Dan", "Stuart"};

// int select_application(){
//     char* selected_app_c=malloc(sizeof(char*));
//     printf("Select an application:\\n");
//     for (int i = 0; i < NUM_APPS; i++) {
//         printf("[%d] %s\n", i, Application_List[i]);
//     }
//     // Get user input
//     int selected_app = -1;
//     while (selected_app < 0 || selected_app >= NUM_APPS) {
//         printf("Enter the number corresponding to the application: ");
//         fgets(selected_app_c, sizeof(selected_app_c), stdin);
// 	    fflush(stdin);
// 	    selected_app = *selected_app_c - '0';
//         if (selected_app < 0 || selected_app >= NUM_APPS) {
//             printf("Invalid selection. Please try again.\n");
//         }
//     }   

// printf("Selected Application: %s\n", Application_List[selected_app]);
// free(selected_app_c);
// return selected_app;
// }

int select_address(char* rec_name){
    char* selected_address_c = malloc(sizeof(char*));
    printf("Select a recipient: \n");
    for (int i = 0; i < NUM_ADDRESSES; i++) {
        printf("[%d] %s\n", i, Address_Tenents[i]);
    }
    // Get user input
    int selected_address = -1;
    while (selected_address < 0 || selected_address >= NUM_ADDRESSES) {
        printf("Enter the number corresponding to the address: ");
        fgets(selected_address_c, sizeof(selected_address_c), stdin);
        fflush(stdin);
        selected_address = *selected_address_c - '0';
        if (selected_address < 0 || selected_address >= NUM_ADDRESSES) {
            printf("Invalid selection. Please try again.\n");
        }
    }

    //printf("Selected Address: %s\n", Address_Book[selected_address]);
    rec_name = Address_Tenents[selected_address];
    
    return (uint8_t)selected_address;
}
