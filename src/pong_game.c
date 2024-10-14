/* CURRENT LIKELY PROBLEMS
- when setting the value of data to be sent across wire, look at better way to set value of single uint8_t* to be a uint8_t.


*/





/*
* This pong game originated from code written by @vicentebolea on github.
* To see the original project, visit https://github.com/vicentebolea/Pong-curses/blob/master/pong.c
*/
#include <unistd.h>
#include <ncurses.h>
#include <pthread.h>
#include "pong_game.h"
#include "selection.h"


/*
* send_update just updates the data of the sendable packet. This a
*/
uint8_t send_update(struct AppData* app_data, uint8_t data)
{
  struct Packet* packet = app_data->sent_packet;
  packet->dlength = (size_t)1;
  //Put the remaining data into the newpack->data spot.
  packet->data[0] = data;
  return packet->data[0];
}

uint8_t check_data(struct AppData* app_data){
  struct Packet* packet = app_data->received_packet;
  if(packet->data[0] != 0x00 && packet->dlength==1)
  {
    uint8_t temp = packet->data[0];
    packet->data[0] = 0x00;
    return temp;
  }
    return 0x00;
}

