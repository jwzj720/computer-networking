#include <pigpiod_if2.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
 
int main(int argc, char *argv[])
{
    // Declare GPIO pin to connect to
    unsigned int LED_GPIO_PIN = 4
    unsigned int pinit = pigpio_start(NULL, NULL)

    if (pinit < 0)
    {
    fprintf(stderr, "pigpio initialisation failed\n");
    return -1;
    }
    else
    {
    print("pigpio initialisation okay\n");
    }

    // Start GPIO connection (Pigpio_start)


    // Configure LED GPIO pin as an output pin
    status = set_mode(pinit,LED_GPIO_PIN, PI_OUTPUT)
    if (status == 0)
    {
        printf("0 status received, success!!")
    }

    // Run program
    while (true)
    {
        // Write out 1 to the LED_GPIO_PIN
        gpio_write(pinit,LED_GPIO_PIN,1)
        // Wait for 2 seconds
        sleep(2)
        // Write out 0 to the LED_GPIO_PIN
        gpio_write(pinit,LED_GPIO_PIN,0)
    }
    return 0
}




 




