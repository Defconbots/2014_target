#include "global.h"
#include "tcs3414_color_sensor.h"
#include "i2c.h"
#include "delay.h"
#include "schedule.h"

#define TCS3414_ADDRESS                           0x39
#define TCS3414_WRITE_ADDRESS                     (TCS3414_ADDRESS << 1)
#define TCS3414_READ_ADDRESS                      (TCS3414_ADDRESS << 1 | 1)

#define TCS3414_COMMAND_BIT                       0x80
#define TCS3414_WORD_BIT                          0x20

#define TCS3414_REGISTER_CONTROL                  0x00
#define TCS3414_REGISTER_TIMING                   0x01
#define TCS3414_REGISTER_INTERRUPT                0x02
#define TCS3414_REGISTER_INTSOURCE                0x03
#define TCS3414_REGISTER_PARTNO_REVID             0x04
#define TCS3414_REGISTER_GAIN                     0x07
#define TCS3414_REGISTER_LOWTHRESHOLD_LOWBYTE     0x08
#define TCS3414_REGISTER_LOWTHRESHOLD_HIGHBYTE    0x09
#define TCS3414_REGISTER_HIGHTHRESHOLD_LOWBYTE    0x0A
#define TCS3414_REGISTER_HIGHTHRESHOLD_HIGHBYTE   0x0B
#define TCS3414_REGISTER_GREENLOW                 0x10
#define TCS3414_REGISTER_GREENHIGH                0x11
#define TCS3414_REGISTER_REDLOW                   0x12
#define TCS3414_REGISTER_REDHIGH                  0x13
#define TCS3414_REGISTER_BLUELOW                  0x14
#define TCS3414_REGISTER_BLUEHIGH                 0x15
#define TCS3414_REGISTER_CLEARLOW                 0x16
#define TCS3414_REGISTER_CLEARHIGH                0x17

//Control Register
#define TCS3414_CONTROL_POWERON                   0x01
#define TCS3414_CONTROL_POWEROFF                  0x00
#define TCS3414_CONTROL_ADC_EN                    0x02
#define TCS3414_CONTROL_ADC_VALID                 0x10

//Gain Register
#define TCS3414_GAIN_GAINMASK                     0x30
#define TCS3414_GAIN_PRESCALARMASK                0x07
#define TCS3414_GAIN_1X                           0x00
#define TCS3414_GAIN_4X                           0x10
#define TCS3414_GAIN_16X                          0x20
#define TCS3414_GAIN_64X                          0x30

//Timing Register
#define TCS3414_INTEGRATION_TIME_MASK             0x03
#define TCS3414_INTEGRATION_TIME_12MS             0x00
#define TCS3414_INTEGRATION_TIME_100MS            0x01
#define TCS3414_INTEGRATION_TIME_400MS            0x02

uint8_t ReadByte(uint8_t command)
{
    I2cStart();
    I2cWrite(TCS3414_WRITE_ADDRESS);
    I2cWrite(command);
    I2cStop();
    I2cStart();
    I2cWrite(TCS3414_READ_ADDRESS);
    uint16_t ret = I2cRead();
    I2cStop();
    return ret;
}
void WriteByte(uint8_t command, uint8_t value)
{
    I2cStart();
    I2cWrite(TCS3414_WRITE_ADDRESS);
    I2cWrite(command);
    I2cWrite(value);
    I2cStop();
}

void Tcs3414Init(void)
{
    // Turn off
    //WriteByte(TCS3414_REGISTER_CONTROL | TCS3414_COMMAND_BIT, TCS3414_CONTROL_POWEROFF);
    //Delay(10 * _MILLISECOND);
    // Turn on
    WriteByte(TCS3414_REGISTER_CONTROL | TCS3414_COMMAND_BIT, TCS3414_CONTROL_POWERON);
    // Integration time to 100ms
    WriteByte(TCS3414_REGISTER_TIMING | TCS3414_COMMAND_BIT, TCS3414_INTEGRATION_TIME_12MS);
    // ADC gain to 4X
    WriteByte(TCS3414_REGISTER_GAIN | TCS3414_COMMAND_BIT, TCS3414_GAIN_4X);
    // Turn on the ADC.
    WriteByte(TCS3414_REGISTER_CONTROL | TCS3414_COMMAND_BIT, (TCS3414_CONTROL_POWERON | TCS3414_CONTROL_ADC_EN));
}

void Tcs3414Shutdown(void)
{
    WriteByte(TCS3414_REGISTER_CONTROL | TCS3414_COMMAND_BIT, TCS3414_CONTROL_POWEROFF);
}

ColorReading Tcs3414ReadAllColors(void)
{
    ColorReading color;
    color.red   = Tcs3414ReadColor(COLOR_RED);
    color.green = Tcs3414ReadColor(COLOR_GREEN);
    color.blue  = Tcs3414ReadColor(COLOR_BLUE);
    color.clear = Tcs3414ReadColor(COLOR_CLEAR);
    return color;
}

uint16_t Tcs3414ReadColor(enum Color c)
{
    uint8_t low = 0;
    uint8_t high = 0;
    switch (c)
    {
        case COLOR_RED:
        {
            low  = ReadByte(TCS3414_REGISTER_REDLOW  | TCS3414_COMMAND_BIT);
            high = ReadByte(TCS3414_REGISTER_REDHIGH | TCS3414_COMMAND_BIT);
            break;
        }
        case COLOR_GREEN:
        {
            low  = ReadByte(TCS3414_REGISTER_GREENLOW  | TCS3414_COMMAND_BIT);
            high = ReadByte(TCS3414_REGISTER_GREENHIGH | TCS3414_COMMAND_BIT);
            break;
        }
        case COLOR_BLUE:
        {
            low  = ReadByte(TCS3414_REGISTER_BLUELOW  | TCS3414_COMMAND_BIT);
            high = ReadByte(TCS3414_REGISTER_BLUEHIGH | TCS3414_COMMAND_BIT);
            break;
        }
        case COLOR_CLEAR:
        {
            low  = ReadByte(TCS3414_REGISTER_CLEARLOW  | TCS3414_COMMAND_BIT);
            high = ReadByte(TCS3414_REGISTER_CLEARHIGH | TCS3414_COMMAND_BIT);
            break;
        }
    }
    return ((high << 8) | low);
}


