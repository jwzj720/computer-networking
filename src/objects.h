#ifndef OBJECTS_H
#define OBJECTS_H

#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#define NUM_GPIO_PAIRS 4 //Number of device ports

/*
 * ReadData struct is implemented at link layer. Used to read correct voltage changes 
 * in order to decode bits.
 */
struct ReadData{
    int id; /* the ID of the user from which data is being read */
    uint32_t READRATE;
    uint32_t ptime;
    uint32_t tick1;
    int rateset;
    int counter;
    int values;
    int run;
    uint8_t* data;
};

/*
 * Packet structure. Requires dlength, sending_addy and receiving_addy to be a valid packet.
*/
struct Packet{
    size_t dlength; // Number of bytes in data
    uint8_t sending_addy; 
    uint8_t receiving_addy;
    uint8_t* data;
};

struct User { /* address book: */
    struct User *next; /* next entry in chain */
    uint8_t ID; /* Id described in message */
    int GPIO_OUT; /* Port to write out packets to */
    int GPIO_IN; /* Port Receiving from */
};

struct GPIO_Pair {
    int gpio_in;
    int gpio_out;
};

struct AppData {
    int pinit;
    int selected_application; // 
    int selected_recipient;  // not sure if we need this?
    struct Packet* sent_packet;
    struct Packet* received_packet;
};

typedef struct{
  short int x, y, c;

  bool movver, movhor; 
} object;

#endif
