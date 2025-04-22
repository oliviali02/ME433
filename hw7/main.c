#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "font.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

// define built in LED of PICO
#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif

// function protoypes
int pico_led_init(void);
void pico_set_led(bool led_on);

void drawLetter(int x, int y, char c);
void drawMessage(int x, int y, char * m);


int main()
{
    // initialize the built in LED
    int rc = pico_led_init();
    hard_assert(rc == PICO_OK);

    stdio_init_all();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    ssd1306_setup();
    ssd1306_clear();
    ssd1306_update();

    // variables for tracking the heartbeat LED
    bool led_on = false;
    absolute_time_t last_heartbeat = get_absolute_time();
    uint64_t t1 = to_us_since_boot(last_heartbeat);

    while (true) {
        absolute_time_t cur_time = get_absolute_time();
        uint64_t t2 = to_us_since_boot(cur_time);
        if ((t2 - t1) > 500e3) {
            led_on = !led_on;
            pico_set_led(led_on);
            t1 = t2;
        }

        // printf("Hello, world!\n");
        // sleep_ms(1000);
        // unsigned int t1 = to_us_since_boot(get_absolute_time());  

        char message[50];
        ssd1306_clear(); 
        sprintf(message, "hello");
        drawMessage(5, 5, message);
        ssd1306_update();  

        // unsigned int t2 = to_us_since_boot(get_absolute_time());  
        // unsigned int tdiff = t2 - t1;
        // convert tdiff to frames per second
        sleep_ms(1000);
    }
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

void drawLetter(int x, int y, char c) {

    for (int j = 0; j < 5; j ++ ) {
        char column = ASCII[c - 0x20][j]; // adjust because ASCII table only has displayable characters

        for (int i = 0; i < 8; i++) {
            char bit = (column >> i) & 0b1; // be careful about ANDing with 1 --> better to use bitwise 1
            ssd1306_drawPixel(x + j, y + i, bit);
        }
    }   
}

void drawMessage(int x, int y, char * m) {
    int i = 0;

    while(m[i] != 0) {
        drawLetter(x + i * 5, y, m[i]);
        i++;
    }
}
