#include <pigpiod_if2.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    int LED_GPIO_PIN = 23;
    int GPIO_RECEIVE = 20;
    int pinit = pigpio_start(NULL,NULL);

    if (pinit<0)
    {
        printf("failed start");
        return -1;
    }
    else
    {
        printf("initialization success\n")
    }

    int status = set_mode(pinit,LED_GPIO_PIN,PI_OUTPUT)
    
    if (status==0)
    {
        printf("0 status received\n");
    }

    for (int i=0;i<10;i++)
    {
        gpio_write(pinit,LED_GPIO_PIN,1);
        printf("%d\n",gpio_read(pinit,GPIO_RECEIVE));
        time_sleep(2);
        gpio_write(pinit,LED_GPIO_PIN,0);
        printf("%d\n",gpio_read(pinit,GPIO_RECEIVE));

        time_sleep(2)
    }
    return 0;
}