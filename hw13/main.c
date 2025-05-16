#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "mpu.h"
#include "ssd1306.h"
#include "font.h"

#include <math.h>


// I2C defines
// This example will use I2C1 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT_IMU i2c1
#define I2C_SDA_IMU 18
#define I2C_SCL_IMU 19

#define ADDR 0x68

#define I2C_PORT_DIS i2c0
#define I2C_SDA_DIS 16
#define I2C_SCL_DIS 17

typedef struct {
    float ax, ay, az;
    float gx, gy, gz;
    float temp;
} IMU_Data;

void writeConfig(unsigned char reg, unsigned char value);
float shiftData(uint8_t val1, uint8_t val2);
IMU_Data readData(unsigned char reg);

void drawLetter(int x, int y, char c);
void drawMessage(int x, int y, char * m);
void drawLines(float x, float y);


int main()
{
    stdio_init_all();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT_IMU, 400*1000);
    
    gpio_set_function(I2C_SDA_IMU, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_IMU, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_IMU);
    gpio_pull_up(I2C_SCL_IMU);

    i2c_init(I2C_PORT_DIS, 400*1000);
    
    gpio_set_function(I2C_SDA_DIS, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_DIS, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_DIS);
    gpio_pull_up(I2C_SCL_DIS);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    // IMU initialization
    writeConfig(PWR_MGMT_1, 0x00); // turn on MPU
    writeConfig(ACCEL_CONFIG, 0b00000111); // enable accelerometer 
    writeConfig(GYRO_CONFIG, 0b00011111); // enable gyroscope

    // OLED screen initialization 
    ssd1306_setup();
    ssd1306_clear();
    ssd1306_update();

    while (true) {
        IMU_Data cur_vals = readData(ACCEL_XOUT_H);
        float x_accel = cur_vals.ax;
        float y_accel = cur_vals.ay;
        float z_accel = cur_vals.az;
        printf("x: %.2f, y: %.2f, z: %.2f\r\n", x_accel, y_accel, z_accel);
        sleep_ms(100);

        drawLines(x_accel, y_accel);
        ssd1306_update(); 
    }
}

void writeConfig(unsigned char reg, unsigned char value) {
    // uint8_t addr = address;

    uint8_t buff[2];
    size_t len = 2;
    buff[0] = reg;
    buff[1] = value;

    i2c_write_blocking(I2C_PORT_IMU, ADDR, buff, len, false);

}

float shiftData(uint8_t high, uint8_t low) {
    return (int16_t) ((high << 8) | low);
}

// should have a buffer that can read all the values at once
IMU_Data readData(unsigned char reg) {
    int len = 14;
    uint8_t buff[len];

    i2c_write_blocking(I2C_PORT_IMU, ADDR, &reg, 1, true);  // true to keep master control of bus
    i2c_read_blocking(I2C_PORT_IMU, ADDR, buff, len, false);  // false - finished with bus

    IMU_Data values;
    values.ax = shiftData(buff[0], buff[1]) * 0.000061;
    values.ay = shiftData(buff[2], buff[3]) * 0.000061;
    values.az = shiftData(buff[4], buff[5]) * 0.000061;

    values.temp = (shiftData(buff[6], buff[7])/340.00) + 36.53;

    values.gx = shiftData(buff[8], buff[9]) * 0.007630;
    values.gy = shiftData(buff[10], buff[11]) * 0.007630;
    values.gz = shiftData(buff[12], buff[13]) * 0.007630;

    return values;
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

void drawLines(float x, float y) {
    ssd1306_clear();
    ssd1306_drawPixel(60, 20, 1);

    float x_length = fabs(x) * 50;
    float y_length = fabs(y) * 50;


    for (int i = 0; i < x_length; i++) {
        if (x < 0) {
            ssd1306_drawPixel(60 + i, 20, 1);
        } else {
            ssd1306_drawPixel(60 - i, 20, 1);
        }
    }

    for (int i = 0; i < y_length; i++) {
        if (y < 0) {
            ssd1306_drawPixel(60, 20 - i, 1);
        } else {
            ssd1306_drawPixel(60, 20 + i, 1);
        }
    }
    
}