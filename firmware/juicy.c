#include "global.h"
#include "juicy.h"
#include "delay.h"
#include "hardware_init.h"
#include "hw.h"

#define JUICY_LED_DELAY 3

void JuicyBlueOn(void)
{
    uint16_t duty = 0;
    uint16_t i = 0;
    for (duty = 0;duty < 100;duty++)
    {
        for (i = 0;i < JUICY_LED_DELAY;i++)
        {
            BLUE_ON();
            DumbDelay(duty);
            BLUE_OFF();
            DumbDelay(100 - duty);
        }
    }
    BLUE_ON();
}

void JuicyBlueOff(void)
{
    uint16_t duty = 0;
    uint16_t i = 0;
    for (duty = 100;duty > 0;duty--)
    {
        for (i = 0;i < JUICY_LED_DELAY;i++)
        {
            BLUE_ON();
            DumbDelay(duty);
            BLUE_OFF();
            DumbDelay(100 - duty);
        }
    }
    BLUE_OFF();
}

void JuicyRedOn(void)
{
    uint16_t duty = 0;
    uint16_t i = 0;
    for (duty = 0;duty < 100;duty++)
    {
        for (i = 0;i < JUICY_LED_DELAY;i++)
        {
            RED_ON();
            DumbDelay(duty);
            RED_OFF();
            DumbDelay(100 - duty);
        }
    }
    RED_ON();
}

void JuicyRedOff(void)
{
    uint16_t duty = 0;
    uint16_t i = 0;
    for (duty = 100;duty > 0;duty--)
    {
        for (i = 0;i < JUICY_LED_DELAY;i++)
        {
            RED_ON();
            DumbDelay(duty);
            RED_OFF();
            DumbDelay(100 - duty);
        }
    }
    RED_OFF();
}

void JuicyBothOn(void)
{
    uint16_t duty = 0;
    uint16_t i = 0;
    for (duty = 0;duty < 100;duty++)
    {
        for (i = 0;i < JUICY_LED_DELAY;i++)
        {
            RED_ON();
            BLUE_ON();
            DumbDelay(duty);
            RED_OFF();
            BLUE_OFF();
            DumbDelay(100 - duty);
        }
    }
    RED_ON();
    BLUE_ON();
}

void JuicyBothOff(void)
{
    uint16_t duty = 0;
    uint16_t i = 0;
    for (duty = 100;duty > 0;duty--)
    {
        for (i = 0;i < JUICY_LED_DELAY;i++)
        {
            RED_ON();
            BLUE_ON();
            DumbDelay(duty);
            RED_OFF();
            BLUE_OFF();
            DumbDelay(100 - duty);
        }
    }
    RED_OFF();
    BLUE_OFF();
}
