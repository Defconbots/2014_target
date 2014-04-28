/**
@file delay.c
@brief Simple delay using global timer
@author Joe Brown
*/
#include "global.h"
#include "delay.h"
#include "schedule.h"

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Delay(uint32_t delay_time)
{
    uint32_t start_time = TimeNow();
    uint32_t end_time = start_time + (delay_time * _MILLISECOND);
    while(TimeNow() < end_time);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void DumbDelay(uint32_t delay_time)
{
    volatile uint32_t cnt = 0;
    while(cnt++ < delay_time);
}
