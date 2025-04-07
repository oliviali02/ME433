/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif

#ifndef LED_DELAY_MS
#define LED_DELAY_MS 1000
#endif

#define GPIO_WATCH_PIN 22

static char event_str[128];
static volatile int count = 0;

static uint32_t last_event_time_us = 0;
static const uint32_t debounce_time_us = 50000; // 50 ms

// Perform initialisation
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

    // Turn the led on or off
void pico_set_led(bool led_on) {
#if defined(PICO_DEFAULT_LED_PIN)
    // Just set the GPIO on or off
    gpio_put(PICO_DEFAULT_LED_PIN, led_on);
#elif defined(CYW43_WL_GPIO_LED_PIN)
    // Ask the wifi "driver" to set the GPIO on or off
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
#endif
}

void gpio_callback(uint gpio, uint32_t events) {
    // debouncing 
    uint32_t now = time_us_32();
    
    if ((now - last_event_time_us) < debounce_time_us) {
        // ignore if it's within the debounce window
        return;
    }
    
    last_event_time_us = now;  // Update last valid event time

    if (count % 2 == 0) {
        pico_set_led(true);
    } else {
        pico_set_led(false);
    }
    count++;

    // print out how many times the button has been pushed
    printf("Count: %d\r\n", count);
}


int main() {
    stdio_init_all();

    gpio_init(GPIO_WATCH_PIN);
    gpio_set_irq_enabled_with_callback(GPIO_WATCH_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    int rc = pico_led_init();
    hard_assert(rc == PICO_OK);
    pico_set_led(false);
    // Wait forever
    while (1);
}
 
