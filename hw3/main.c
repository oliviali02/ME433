#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"


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

    // initialize adc
    adc_init(); // init the adc module
    adc_gpio_init(26); // set ADC0 pin to be adc input instead of GPIO
    adc_select_input(0); // select to read from ADC0

    while (1) {
        if (gpio_get(22) == 0) {
            gpio_put(25, 1);
        } else {
            gpio_put(25, 0);
        }

        uint16_t result = adc_read();
        printf("adc value: %d\r\n", result);
        // char message[100];
        // scanf("%s", message);
        // printf("message: %s\r\n",message);
        // sleep_ms(50);
    }
}
