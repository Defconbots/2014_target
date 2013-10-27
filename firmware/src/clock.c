/**
@file clock.c
@brief Clock config data and functions
@author Joe Brown
*/
#include "global.h"
#include "clock.h"
#include "schedule.h"

// to be externed in global.h so it is accessible to everyone
volatile uint32_t g_clock_speed = 0;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void ClockConfig(uint8_t mhz)
{
    // Calibrated DCO values are 1,8,12,16
#define CLOCK_CASE(x)               \
    case x:                         \
        BCSCTL1 = CALBC1_##x##MHZ;  \
        DCOCTL  = CALDCO_##x##MHZ;  \
        break;

    switch(mhz)
    {
        CLOCK_CASE(1);
        CLOCK_CASE(8);
        CLOCK_CASE(12);
        CLOCK_CASE(16);
        default:
        {
            while(1)
            {
                // If you're here the clock was configured incorrectly
                _NOP();
            }
        }
    }
#undef CLOCK_CASE
    g_clock_speed = mhz * 1000000;

#ifdef ADJUST_SCHEDULER_ON_CLOCK_CONFIG
    ScheduleTimerInit();
#endif
}
