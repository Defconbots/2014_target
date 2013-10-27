#include "global.h"
#include "hw.h"
#include "schedule.h"
#include "clock.h"
#include "delay.h"
#include "hardware_init.h"
#include "interrupt.h"
#include "state.h"

void HwInit(void)
{
    HW_INPUT(LASER_DETECT_1);
    HW_INPUT(LASER_DETECT_2);
    HW_OUTPUT(ILLUMINATE);
    HW_OUTPUT(INDICATE);
}

void hit(void)
{
    static uint32_t hit_time;
    if (!hit_time)
    {
        hit_time = TimeNow();
        InterruptAttach(LASER_DETECT_1,hit,RISING);
        InterruptAttach(LASER_DETECT_2,hit,RISING);
    }
    else
    {
        uint32_t hit_duration = TimeNow() - hit_time;
        if (hit_duration > 300ul * _MILLISECOND)
        {
            StateMachinePublishEvent(&s,OMG_LASER_PEW_PEW);
        }
        InterruptAttach(LASER_DETECT_1,hit,FALLING);
        InterruptAttach(LASER_DETECT_2,hit,FALLING);
        hit_time = 0;
    }
}

enum Event
{
    DEFAULT_EVENTS,     // Enter, idle, exit
    OMG_LASER_PEW_PEW,
    HIT_TIMEOUT
};

State idle;
State i_am_shot;
StateMachine s;

Transition rules[] =
{
    {idle,      OMG_LASER_PEW_PEW,   hit},
    {i_am_shot, HIT_TIMEOUT,         idle}
};


void main(void)
{
    WD_STOP();
    ClockConfig(16);
    ScheduleTimerInit();
    HwInit();
    s = StateMachineCreate(rules, sizeof(rules),idle);
    _EINT();

    while (1)
    {
        StateMachineRun(&s);
    }
}

void idle(uint8_t ev)
{
    switch (ev)
    {
        case ENTER:
        {
            InterruptAttach(LASER_DETECT_1,hit,FALLING);
            InterruptAttach(LASER_DETECT_2,hit,FALLING);
            ILLUMINATE_ON();
            break;
        }
        case EXIT:
        {
            InterruptDetach(LASER_DETECT_1);
            InterruptDetach(LASER_DETECT_2);
            ILLUMINATE_OFF();
            break;
        }
    }
}

void i_am_shot(uint8_t ev)
{
    static uint8_t hit_count;
    switch (ev)
    {
        case ENTER:
        {
            hit_count = (hit_count < 3) ? hit_count + 1 : 0;
            break;
        }
        case IDLE:
        {
            break;
        }
        case EXIT:
        {
            break;
        }
    }    
}
