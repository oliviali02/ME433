/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/adc.h"

#define FLAG_VALUE 123

// define built in LED of PICO
#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif

static volatile int message;

static volatile int data_01;
static volatile int data_10;
 
// function prototypes
int pico_led_init(void);
void pico_set_led(bool led_on);

void core1_entry();

// main loop
int main() {
    stdio_init_all();

    // initialize the built in LED
    int rc = pico_led_init();
    hard_assert(rc == PICO_OK);

    // init the ADC
    adc_init(); // init the adc module
    adc_gpio_init(26); // set ADC0 pin to be adc input instead of GPIO
    adc_select_input(0); // select to read from ADC0

    // wait until the port is opened
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    
    printf("Hello, multicore!\n");

    /// \tag::setup_multicore[]

    multicore_launch_core1(core1_entry);

    // // Wait for it to start up

    // // core0 waiting for message from core1
    // uint32_t g = multicore_fifo_pop_blocking();

    // // is this the expected value?
    // if (g != FLAG_VALUE) 
    //     printf("Hmm, that's not right on core 0!\n");
    // else {
    //     multicore_fifo_push_blocking(FLAG_VALUE);
    //     printf("It's all gone well on core 0!\r\n");
    // }



    while (1) {
        printf("Send a 0 to read voltage on A0, 1 to turn on the LED, or 2 to turn off the LED\r\n");
        scanf("%d", &message);
        printf("You wrote the command %d\r\n", message);

        // push to core1 a command
        multicore_fifo_push_blocking(message);

        // pop from core1
        uint32_t g = multicore_fifo_pop_blocking();

        // print the value depending on what you got from pop
        if (g == 0) {
            float voltage = (float) (data_10/4095.0) * 3.3;
            printf("The voltage is %.2f volts\r\n", voltage);
        } else if (g == 1) {
            pico_set_led(true);
        } else if (g == 2) {
            pico_set_led(false);
        }
        
        tight_loop_contents(); // for core0 to stay on
    }
        

    /// \end::setup_multicore[]
}

// initialize the LED
int pico_led_init(void) {
    #if defined(PICO_DEFAULT_LED_PIN)
        // A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
        // so we can use normal GPIO functionality to turn the led on and off
        gpio_init(PICO_DEFAULT_LED_PIN);
        gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
        return PICO_OK;
    #elif defined(CYW43_WL_GPIO_LED_PIN)
        // For Pico W devices we need to initialise the driver etc
        return cyw43_arch_init();
    #endif
}

// turn the led on or off
void pico_set_led(bool led_on) {
    #if defined(PICO_DEFAULT_LED_PIN)
        // Just set the GPIO on or off
        gpio_put(PICO_DEFAULT_LED_PIN, led_on);
    #elif defined(CYW43_WL_GPIO_LED_PIN)
        // Ask the wifi "driver" to set the GPIO on or off
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
    #endif
}

void core1_entry() {

    while (1) {
        tight_loop_contents(); // for core1 to stay on

        // pop from core 0
        uint32_t g = multicore_fifo_pop_blocking();

        if (g == 0) {
            // read the ADC value
            uint16_t result = adc_read();

            // put ADC value into data_10
            data_10 = (int) result;
    
            // push a command to core0
            multicore_fifo_push_blocking(0);

        } else if (g == 1) {
            // push a command to core0
            multicore_fifo_push_blocking(1);

        } else if (g == 2)  {
            // push a command to core0
            multicore_fifo_push_blocking(2);
        }
    }
}