#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <pigpiod_if2.h>
#include "text_7bit.h"
#include "link_receive.h"

pthread_t* reading_thread;
pthread_t* write_thread;
pthread_t* main_thread;

//
void read_thread(void* arg)
{
    
    read_to_file();
    return;
}

void send_thread(void* arg)
{
    send_to_file();
    return;
    
}

int main()
{
    reading_thread = start_thread(read_thread,NULL);
    write_thread = start_thread(send_thread,NULL);

    while(1)
    {

        fflush(stdout);
    }

    pthread_join(reading_thread, NULL);
    pthread_join(write_thread, NULL);
}