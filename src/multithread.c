#include "link_receive.h"
#include "link_send.h"

pthread_t* reading_thread;
pthread_t* write_thread;
pthread_t* main_thread;

//
void* read_thread()
{
    
    read_to_file();
    return NULL;
}

void* send_thread()
{
    send_to_file();
    return NULL;
    
}

int main()
{
    reading_thread = start_thread(*read_thread,NULL);
    write_thread = start_thread(*send_thread,NULL);

    while(1)
    {

        fflush(stdout);
    }

    pthread_join(*reading_thread, NULL);
    pthread_join(*write_thread, NULL);
}
