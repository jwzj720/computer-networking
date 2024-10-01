// main.c
#include "link_receive.h"
#include "link_send.h"
#include <pthread.h>

pthread_t reading_thread;
pthread_t write_thread;

pthread_mutex_t send_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t read_mutex = PTHREAD_MUTEX_INITIALIZER;

void* read_thread(void* arg)
{
    while(1)
    {
        pthread_mutex_lock(&read_mutex);
        read_to_file();
        pthread_mutex_unlock(&read_mutex);
    }
    return NULL;
}

void* send_thread(void* arg)
{
    while(1)
    {
        pthread_mutex_lock(&send_mutex);
        send_to_file();
        pthread_mutex_unlock(&send_mutex);
    }
    return NULL;
}

int main()
{
    if(pthread_create(&reading_thread, NULL, read_thread, NULL) != 0) {
        perror("Could not create reading thread");
        return 1;
    }

    if(pthread_create(&write_thread, NULL, send_thread, NULL) != 0) {
        perror("Could not create writing thread");
        return 1;
    }

    pthread_join(reading_thread, NULL);
    pthread_join(write_thread, NULL);

    return 0;
}
