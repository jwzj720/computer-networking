# Notes page for the Pigpiod functions
Understanding the capabilities of the library we are using is important, so this document is a compiled list of the different functions, but specifically how we should use them and what they accomplish for our purposes.

# Basics
* Fundamental functions for sending and reading bits over GPIO
## pigpio_start
Declare a gpio connection. This gets used as the _pi_ parameter for many functions to confirm a connection was made
* For our experiements, we can use `(NULL,NULL)` for the parameters as we are not using ports or IPs.
### Usage
``int pinit = pigpio_start(NULL,NULL);``

## pigpio_stop
Stops a gpio conneciton as created by `pigpio_start`. Takes start var as parameter.
### Usage
``pigpio_stop(pinit);``

## set_mode/get_mode
Sets a specified GPIO port to a input or output mode, or gets a port's mode
```
int LED_GPIO_PIN = 23;
int status = set_mode(pinit,LED_GPIO_PIN,PI_OUTPUT);
get_mode(pinit,LED_GPIO_PIN);
```

## gpio_read/gpio_write
Read/write the gpio level at specified port. This reads/write's either a 1 or 0.
```
gpio_write(pinit,LED_GPIO_PIN,1);
printf("%d\n",gpio_read(pinit,GPIO_RECEIVE));
```

# PWM functions
PWM stands for "Pulse Frequency Modulation" and is used to control average power or amplitude delivered by an electrical signal. However, it seems to utilize negative voltages when creating smooth waveforms. 
# Servo

# I2C

# SPI





