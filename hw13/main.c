#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "mpu.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

#define ADDR 0x68

typedef struct {
    float ax, ay, az;
    float gx, gy, gz;
    float temp;
} IMU_Data;

void writeConfig(unsigned char reg, unsigned char value);
float shiftData(uint8_t val1, uint8_t val2);
IMU_Data readData(unsigned char reg);


int main()
{
    stdio_init_all();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c


    writeConfig(PWR_MGMT_1, 0x00); // turn on MPU
    writeConfig(ACCEL_CONFIG, 0b00000000); // enable accelerometer 
    writeConfig(GYRO_CONFIG, 0b00011000); // enable gyroscope

    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}

void writeConfig(unsigned char reg, unsigned char value) {
    // uint8_t addr = address;

    uint8_t buff[2];
    size_t len = 2;
    buff[0] = reg;
    buff[1] = value;

    i2c_write_blocking(I2C_PORT, ADDR, buff, len, false);

}

float shiftData(uint8_t val1, uint8_t val2) {
    return (val1 << 8) | val2;
}

// should have a buffer that can read all the values at once
IMU_Data readData(unsigned char reg) {
    int len = 14;
    uint8_t buff[len];

    i2c_write_blocking(I2C_PORT, ADDR, &reg, 1, true);  // true to keep master control of bus
    i2c_read_blocking(I2C_PORT, ADDR, buff, len, false);  // false - finished with bus

    IMU_Data values;
    values.ax = shiftData(buff[0], buff[1]);
    values.ay = shiftData(buff[2], buff[3]);
    values.az = shiftData(buff[4], buff[5]);

    values.gx = shiftData(buff[6], buff[7]);
    values.gy = shiftData(buff[8], buff[9]);
    values.gz = shiftData(buff[10], buff[11]);

    values.temp = shiftData(buff[12], buff[13]);

    return values;
}