<<<<<<< HEAD
#include "link_receive.h"
#include "link_send.h"
=======
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <pigpiod_if2.h>
#include "text_7bit.h"
#include "link_receive.h"
>>>>>>> c2d5ea376abc7551edae591c69ed932c0ecf0a24

pthread_t* reading_thread;
pthread_t* write_thread;
pthread_t* main_thread;

//
<<<<<<< HEAD
void* read_thread()
{
    
    read_to_file();
    return NULL;
}

void* send_thread()
{
    send_to_file();
    return NULL;
=======
void read_thread(void* arg)
{
    
    read_to_file();
    return;
}

void send_thread(void* arg)
{
    send_to_file();
    return;
>>>>>>> c2d5ea376abc7551edae591c69ed932c0ecf0a24
    
}

int main()
{
<<<<<<< HEAD
    reading_thread = start_thread(*read_thread,NULL);
    write_thread = start_thread(*send_thread,NULL);
=======
    reading_thread = start_thread(read_thread,NULL);
    write_thread = start_thread(send_thread,NULL);
>>>>>>> c2d5ea376abc7551edae591c69ed932c0ecf0a24

    while(1)
    {

        fflush(stdout);
    }

<<<<<<< HEAD
    pthread_join(*reading_thread, NULL);
    pthread_join(*write_thread, NULL);
}
=======
    pthread_join(reading_thread, NULL);
    pthread_join(write_thread, NULL);
}
>>>>>>> c2d5ea376abc7551edae591c69ed932c0ecf0a24
