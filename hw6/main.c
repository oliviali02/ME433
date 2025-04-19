#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

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

#ifndef LED_DELAY_MS
#define LED_DELAY_MS 500
#endif

static uint8_t address = 0b0100000;
// function protoypes
int pico_led_init(void);
void pico_set_led(bool led_on);

void mcp_init();
void setPin(unsigned char address, unsigned char register, unsigned char value);
unsigned char readPin(unsigned char address, unsigned char register);

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

    // intialize the MCP23008
    mcp_init();

    // variables for tracking the heartbeat LED
    bool led_on = false;
    absolute_time_t last_heartbeat = get_absolute_time();
    uint64_t t1 = to_us_since_boot(last_heartbeat);

    unsigned char read_value;

    while (true) {
        absolute_time_t cur_time = get_absolute_time();
        uint64_t t2 = to_us_since_boot(cur_time);
        if ((t2 - t1) > 500e3) {
            led_on = !led_on;
            pico_set_led(led_on);
            t1 = t2;
        }
        
        read_value = readPin(address, 0x09); // read button pin
        if ((read_value & 1) == 0) { // button is pushed
            setPin(address, 0x0A, 0b10000000);
        } else { // button not pushed
            setPin(address, 0x0A, 0b00000000);
        }
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

// initialize the i2c chip
void mcp_init() {

    // set input and output pins
    uint8_t buff[2]; 
    size_t len = 2;
    buff[0] = 0x00; // register to change
    buff[1] = 0b01111111; // value being written to register
                          // GP0 = input, GP7 = output

    i2c_write_blocking(I2C_PORT, address, buff, len, false);
}

void setPin(unsigned char addr, unsigned char reg, unsigned char value) {
    // uint8_t addr = address;

    uint8_t buff[2];
    size_t len = 2;
    buff[0] = reg;
    buff[1] = value;

    i2c_write_blocking(I2C_PORT, addr, buff, len, false);

}

unsigned char readPin(unsigned char addr, unsigned char reg) {
    unsigned char buff;

    i2c_write_blocking(I2C_PORT, addr, &reg, 1, true);  // true to keep master control of bus
    i2c_read_blocking(I2C_PORT, addr, &buff, 1, false);  // false - finished with bus

    return buff;
}