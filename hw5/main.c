#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS_DAC 17
#define PIN_CS_RAM 15
#define PIN_SCK  18
#define PIN_MOSI 19

union FloatInt {
    float f;
    uint32_t i;
};

void init_ram();
void ram_write(uint16_t address, float voltage);
float ram_read(uint16_t address);


int main()
{
    stdio_init_all();

    // SPI initialisation. This example will use SPI at 1MHz.
    spi_init(SPI_PORT, 1000*1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS_DAC,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_CS_RAM,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(PIN_CS_DAC, GPIO_OUT);
    gpio_put(PIN_CS_DAC, 1);
    // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi

    init_ram();

    // for i = 0 to 1000
        // calculate v = sin(t)
        // ram_write(address, voltage);

    while (true) {
        // read for one address
            // float val = ram_read(address);
        // send the float to the dac (copy what was done in hw4)
        // wait one ms
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}

void init_ram() {
    uint8_t buff[2];
    // buff[0] = instruction -> name of SFR
    // buff[1] = value -> what you want to change it to
    buff[0] = 0b00000101; // i want to change the status register 
    buff[1] = 0b01000000; // to sequential mode


    // cs low 
    // spi_write_blocking
    // cs high
}

void ram_write(uint16_t address, float voltage) {
    // need to write 7 times 
    uint8_t buff[7];
    buff[0] = 0b00000010; 
    buff[1] = address >> 8;
    buff[2] = address & 0xFF;

    // break up float into 4 bits
    union FloatInt num;
    num.f = voltage;

    buff[3] = num.i>>24; // leftmost 8 bit
    // buff[4] = 
    // buff[5] = 
    // buff[6] =   // right most 8 bit

    // cs low 
    // spi_write_blocking
    // cs high
}

float ram_read(uint16_t address) {
    uint8_t out_buff[7];
    uint8_t in_buff[7];

    out_buff[0]; // instruciton
    out_buff[1]; // address
    out_buff[2]; // address

    // cs low 
    // spi_write_read_blocking
        // wants two buffers (outbuf and inbuf)
        // as it writes element 0 of outbuf, it reads element 0 of inbuf
        // last 4 items of outbuff can be anything
    // cs high

    // num.i = (in_buff[3] << 24) | (in_buff[4] << 16)
    // return num.f in the endf
    return 0.0;
}
