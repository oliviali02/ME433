#include <stdio.h>
#include "pico/stdlib.h"


int main()
{
    stdio_init_all();

    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }

    printf("Start!\n");
    
    // initalize the LED on the PICO
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);

    // initalize the button 
    gpio_init(22);
    gpio_set_dir(22, GPIO_IN);

    while (1) {
        if (gpio_get(22) == 0) {
            gpio_put(25, 1);
        } else {
            gpio_put(25, 0);
        }

        // char message[100];
        // scanf("%s", message);
        // printf("message: %s\r\n",message);
        // sleep_ms(50);
    }
}
