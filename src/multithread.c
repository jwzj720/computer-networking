#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

pthread_t* read_thread;
pthread_t* write_thread;
pthread_t* main_thread;

void run()
{

}

int main()
{
    int* GPIO_RECEIEVE = 26;

    pthread_create(read_thread,NULL,run,GPIO_RECEIEVE);
    while(1)
    {

        fflush(stdout);
    }
}