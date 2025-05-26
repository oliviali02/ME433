#include <stdio.h>
#include "pico/stdlib.h"
#include <stdlib.h>
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"

#define ENABLE 20
#define PHASE 21 

static uint16_t wrap = 1000;
static volatile signed int duty_cycle = 0;

void motor_init() {
    // initialize PWM pin
    gpio_set_function(ENABLE, GPIO_FUNC_PWM); // Set the pin to be PWM

    // initialize direction pin
    gpio_init(PHASE); 
    gpio_set_dir(PHASE, GPIO_OUT);
    gpio_put(PHASE, 0);

    // initialize PWM to 50 Hz
    uint slice_num = pwm_gpio_to_slice_num(ENABLE); // Get PWM slice number

    float div = 1.0; // must be between 1-255
    pwm_set_clkdiv(slice_num, div); // divider

    pwm_set_wrap(slice_num, wrap);

    pwm_set_enabled(slice_num, true); // turn on the PWM
}

void increment_pwm(char sign) {
    if (sign == '+') {
        if (duty_cycle == 100) {
            printf("Duty cycle is already 100, cannot increase any further!");
            return;
        } else {
            duty_cycle += 1;
        }
    } else if (sign == '-') {
        if (duty_cycle == -100) {
            printf("Duty cycle is already -100, cannot decrease any further!");
        } else {
            duty_cycle -= 1;
        }
    } else {
        printf("ERROR: invalid command \r\n");
        return;
    }

    int level = (int)((abs(duty_cycle) / 100.0) * wrap);

    if (duty_cycle > 0) {
        gpio_put(PHASE, 0);
    } else if (duty_cycle < 0 ) {
        gpio_put(PHASE, 1);
    } else {
        level = 0;
    }

    pwm_set_gpio_level(ENABLE, level);
    printf("Duty Cycle = %d\r\n", duty_cycle);
}

int main()
{
    stdio_init_all();
    motor_init();

    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }

    while (true) {
        printf("Enter + to increase duty cycle or - to decrease duty cycle: \r\n");
        char sign;
        scanf("%c", &sign);
        increment_pwm(sign);
    }
}
