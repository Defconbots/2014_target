#include "global.h"
#include "hw.h"
#include "schedule.h"
#include "clock.h"
#include "delay.h"
#include "hardware_init.h"
#include "interrupt.h"
#include "state.h"
#include "tcs3414_color_sensor.h"


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//                        State machine config
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//StateMachine s;

enum Event
{
    DEFAULT_EVENTS,     // Enter, idle, exit
    OMG_LASER_PEW_PEW,
    HIT_TIMEOUT
};

void idle(uint8_t ev);
void i_am_shot(uint8_t ev);


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
    HW_OUTPUT(RED);
    RED_OFF();
    HW_OUTPUT(BLUE);
    BLUE_OFF();

    HW_OUTPUT(SYNC);
    SYNC_LOW();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//                           Interrupt handlers
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void CheckForHit(void)
{

}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//                             State functions
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void idle(uint8_t ev)
{
}

void i_am_shot(uint8_t ev)
{
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//                                  Entry
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void main(void)
{
    WD_STOP();
    ClockConfig(8);
    ScheduleTimerInit();
    HwInit();
    //s = StateMachineCreate(rules,sizeof(rules),idle);
    _EINT();
    Tcs3414Init();
    while (1)
    {
        uint16_t test = Tcs3414ReadColor(COLOR_CLEAR);
        Delay(3000 - test);
        BLUE_TOGGLE();
        //Tcs3414Shutdown();
    }
}
