#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <math.h>

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19


static inline void cs_select(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 0);
    asm volatile("nop \n nop \n nop"); // FIXME
}

static inline void cs_deselect(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 1);
    asm volatile("nop \n nop \n nop"); // FIXME
}

void writeDAC(int channel, float voltage);

int main()
{
    stdio_init_all();

    // SPI initialisation. This example will use SPI at 1MHz.
    spi_init(SPI_PORT, 1000000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);
    // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi

    while (true) {
        float t = 0;

        for (int i = 0; i < 10000; i++) {
            t = t + 0.01;
            
            // 2 Hz sine wave on channel A
            float v_a = 1.65 * sin(4.0*M_PI*t) + 1.65;
            writeDAC(0, v_a);

            // 1 Hz triangle wave on channel B
            // float v_b = (6.6/M_PI) * asin(sin(2*M_PI*t));
            // writeDAC(1, v_a);
            // sleep_ms(10);
        }
    }
}

void writeDAC(int channel, float voltage) {
    uint8_t data[2];
    int len = 2;
   
    // bit shift to change the channel
    uint16_t d = 0;
    d = d | (channel << 15);
    d = d | 0b111 << 12; 
    
    // convert the voltage to a 10 bit value and add it to the data
    uint16_t v = (uint16_t) ((voltage / 3.3) * 1023); 

    d = d | v << 2; 

    data[0] = d >> 8;
    data[1] = d & 0xFF;

    cs_select(PIN_CS);
    spi_write_blocking(SPI_PORT, data, len); // where data is a uint8_t array with length len
    cs_deselect(PIN_CS);
}

