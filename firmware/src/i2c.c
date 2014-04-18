#include "global.h"
#include "hardware_init.h"
#include "../hw.h"
#include "i2c.h"
#include "delay.h"
#include "schedule.h"

// timing assumes 8mhz clock
#define BIT_TIME_5_US   4
#define BIT_TIME_2_US   2
#define DELAY_FULL()   (__delay_cycles(8*BIT_TIME_5_US))
#define DELAY_PART()   (__delay_cycles(8*BIT_TIME_2_US))
#define DELAY_HALF()   (__delay_cycles(8))


void I2cInit(void)
{
}

void I2cStart(void)
{
    DAT_INPUT();
    CLK_INPUT();
    DAT_LOW();
    CLK_LOW();
    DELAY_FULL();
    DAT_OUTPUT();
    DELAY_FULL();
    CLK_OUTPUT();
}

void I2cStop(void)
{
    DAT_OUTPUT();
    DELAY_FULL();
    CLK_INPUT();
    DELAY_FULL();
    DAT_INPUT();
}

uint8_t I2cWrite(uint8_t data)
{
    uint8_t i = 0;
    for(i=0; i < 8; ++i )
    {
        if (data & 0x80)
        {
            DAT_INPUT();
        }
        else
        {
            DAT_OUTPUT();
        }
        data <<= 1;
        CLK_INPUT();
        DELAY_FULL();
        CLK_OUTPUT();
        DELAY_HALF();
    }
    DAT_INPUT();
    CLK_INPUT();
    i = !(DAT_READ());
    if(i)
    {
        DAT_OUTPUT();
    }
    DELAY_HALF();
    CLK_OUTPUT();
    return i;
}

uint8_t I2cRead(void)
{
    uint8_t data = 0;
    DAT_INPUT();
    uint8_t i = 0;
    for(i = 0;i < 8;i++)
    {
        CLK_INPUT();
        data = data << 1;
        DELAY_HALF();
        if(DAT_READ())
        {
            data |= 0x01;
        }
        DELAY_HALF();
        CLK_OUTPUT();
        DELAY_PART();
    }
    return data;
}
