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
    gpio_put(25, 1); // turn on LED
 
    // initalize the button 
    gpio_init(22);
    gpio_set_dir(22, GPIO_IN);

    // wait until button is pressed
    while (gpio_get(22) != 0) {
        sleep_ms(100);
    }

    // turn off LED
    gpio_put(25, 0);

    // initialize adc
    adc_init(); // init the adc module
    adc_gpio_init(26); // set ADC0 pin to be adc input instead of GPIO
    adc_select_input(0); // select to read from ADC0

    while (1) {
        printf("Enter how many analog samples to take (0 - 100): ");
        int num;
        scanf("%d", &num);

        for (int i = 0; i < num; i++) {
            uint16_t result = adc_read();
            float voltage = (float) (result/4095.0) * 3.3;
            printf("adc value: %.2f\r\n", voltage);
            sleep_ms(10);
        }
    }
}
