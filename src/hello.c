#include <stdio.h>
#include <pigpiod_if2.h>

int main()
{
    int test = pigpio_start(NULL, NULL);
    int status = set_mode(test, 4, PI_OUTPUT);
    gpio_write(test, 4, 1);
    printf("Hello, World! %d\n", status);
    print("Our status is %d\n", status);
    for (int i = 0; i < 10; i++)
    {
        int write = gpio_write(test, 4, 1);
        time_sleep(1);
        printf("Write status: %d\n", write);
    }
    pigpio_stop(test);
    return 0;
}