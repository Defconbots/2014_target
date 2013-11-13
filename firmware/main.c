#include "global.h"
#include "hw.h"
#include "schedule.h"
#include "clock.h"
#include "delay.h"
#include "hardware_init.h"
#include "interrupt.h"
#include "state.h"
#include "adc.h"

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//                        State machine config
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
enum Event
{
    DEFAULT_EVENTS,     // Enter, idle, exit
    OMG_LASER_PEW_PEW,
    HIT_TIMEOUT
};

void idle(uint8_t ev);
void i_am_shot(uint8_t ev);

StateMachine s;

Transition rules[] =
{
    {idle,      OMG_LASER_PEW_PEW,   i_am_shot},
    {i_am_shot, HIT_TIMEOUT,         idle}
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//                             Hardware init
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void HwInit(void)
{
    AdcInit();
    HW_INPUT(LASER_DETECT_1);
    HW_INPUT(LASER_DETECT_2);
    HW_OUTPUT(ILLUMINATE);
    ILLUMINATE_ON();
    HW_OUTPUT(INDICATE);
    INDICATE_ON();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//                           Interrupt handlers
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void CheckForHit(void)
{
    static uint32_t laser_hit_count;
    uint16_t sample = AdcRead(LASER_DETECT_1_ADC);
    laser_hit_count = (sample > 400) ? laser_hit_count + 1 : 0;
    if (laser_hit_count > 5)
    {
        StateMachinePublishEvent(&s,OMG_LASER_PEW_PEW);
        laser_hit_count = 0;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//                             State functions
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void idle(uint8_t ev)
{
    switch (ev)
    {
        case ENTER:
        {
        	CallbackMode(CheckForHit,ENABLED);
            ILLUMINATE_ON();
            INDICATE_OFF();
            break;
        }
        case EXIT:
        {
        	CallbackMode(CheckForHit,DISABLED);
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
            uint8_t i = 0;
            hit_count = (hit_count < 3) ? hit_count + 1 : 1;
            for (i = 0;i < hit_count;i++)
            {
                INDICATE_ON();
                Delay(500ul * _MILLISECOND);
                INDICATE_OFF();
                Delay(500ul * _MILLISECOND);
            }
            Delay(500ul * _MILLISECOND);
            break;
        }
        case IDLE:
        {
            StateMachinePublishEvent(&s,HIT_TIMEOUT);
            break;
        }
        case EXIT:
        {
            break;
        }
    }    
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//                                  Entry
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void main(void)
{
    WD_STOP();
    ClockConfig(16);
    ScheduleTimerInit();
    HwInit();
    s = StateMachineCreate(rules, sizeof(rules),idle);
	CallbackRegister(CheckForHit,50ul * _MILLISECOND);
    _EINT();
    Delay(2000ul * _MILLISECOND);

    while (1)
    {
        StateMachineRun(&s);
    }
}
