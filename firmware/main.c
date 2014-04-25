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
//                        Locals and State Machine config
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
StateMachine s;

enum Event
{
    DEFAULT_EVENTS, // Enter, idle, exit
    STUN,           // Verified laser hit
    STUN_TIMEOUT,   // Timeout from hit delay
    CONFIG,         // Hit detected by another target or target reset
    CNT_TICK,       // CNT tick to tell us how many hits to use
    KILL            // When stuns >= kill count
};

void Idle(uint8_t ev);
void Config(uint8_t ev);
void Stunned(uint8_t ev);
void Dead(uint8_t ev);


const Transition rules[] =
{
//  Current State + Event         = New State
    {Idle,          STUN,           Stunned},
    {Idle,          CONFIG,         Config},
    {Config,        CONFIG,         Idle},
    {Config,        KILL,           Dead},
    {Stunned,       STUN_TIMEOUT,   Idle},
    {Stunned,       KILL,           Dead},
    {Dead,          CONFIG,         Config}
};

static uint8_t kill_count = 0xFF;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//                             Hardware init
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void HwInit(void)
{
    // LEDs
    HW_OUTPUT(RED);
    RED_OFF();
    HW_OUTPUT(BLUE);
    BLUE_OFF();

    // TCS3414
    HW_OUTPUT(SYNC);
    SYNC_LOW();

    // Intermodule Communication
    HW_INPUT(SET);
    HW_PULL_UP(SET);
    HW_INPUT(CNT);
    HW_PULL_UP(CNT);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//                              Signal generators
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void CheckForHit(void)
{
    static uint32_t laser_hit_count = 0;
    uint16_t red   = Tcs3414ReadColor(COLOR_RED);
    uint16_t green = Tcs3414ReadColor(COLOR_GREEN);
    uint8_t hit = (red > 50 && green < 200);
    if (hit)
    {
        laser_hit_count++;
    }
    else
    {
        if (laser_hit_count >= 3)// && laser_hit_count < 30) // 300-800ms
        {
            StateMachinePublishEvent(&s,STUN);
        }
        laser_hit_count = 0;
    }
}

void ConfigThrow(void)
{
    static uint8_t edge = 0;
    uint8_t glitch = 0;
    while (glitch++ < 50)
    {
        if ((edge && !SET_READ()) || (!edge && SET_READ()))
        {
            return;
        }
    }
    if (edge)
    {
        InterruptAttach(SET,ConfigThrow,FALLING);
        edge = 0;
    }
    else
    {
        InterruptAttach(SET,ConfigThrow,RISING);
        edge = 1;
    }
    StateMachinePublishEvent(&s,CONFIG);
}

void CntTick(void)
{
    uint8_t glitch = 0;
    while (glitch++ < 50)
    {
        if (CNT_READ())
        {
            return;
        }
    }
    StateMachinePublishEvent(&s,CNT_TICK);
}


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//                             State functions
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Idle(uint8_t ev)
{
    switch (ev)
    {
        case ENTER:
        {
            CallbackMode(CheckForHit,ENABLED);
            BLUE_ON();
            RED_OFF();
            Delay(1000ul * _MILLISECOND);
            break;
        }
        case EXIT:
        {
            CallbackMode(CheckForHit,DISABLED);
            BLUE_OFF();
            break;
        }
    }
}

void Config(uint8_t ev)
{
    switch (ev)
    {
        case ENTER:
        {
            kill_count = 0;
            break;
        }
        case CNT_TICK:
        {
            kill_count++;
            break;
        }
        case EXIT:
        {
            RED_OFF();
            break;
        }
    }
}

void Stunned(uint8_t ev)
{
    switch (ev)
    {
        case ENTER:
        {
            RED_ON();
            HW_OUTPUT(SET);
            HW_SET_LOW(SET);
            //Tcs3414Shutdown();
            kill_count--;
            Delay(3000ul * _MILLISECOND);
            break;
        }
        case IDLE:
        {
            StateMachinePublishEvent(&s, kill_count ? STUN_TIMEOUT : KILL);
            break;
        }
        case EXIT:
        {
            HW_SET_HIGH(SET);
            HW_INPUT(SET);
            //Tcs3414Init();
            break;
        }
    }
}

void Dead(uint8_t ev)
{
    switch (ev)
    {
        case ENTER:
        {
            Tcs3414Shutdown();
            break;
        }
        case EXIT:
        {
            Tcs3414Init();
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
    ClockConfig(8);
    ScheduleTimerInit();
    HwInit();
    Tcs3414Init();
    s = StateMachineCreate(rules,sizeof(rules), Idle);
    CallbackRegister(CheckForHit,100ul * _MILLISECOND);
    InterruptAttach(SET,ConfigThrow,FALLING);
    InterruptAttach(CNT,CntTick,FALLING);
    _EINT();
    Delay(1000ul * _MILLISECOND);
    while (1)
    {
        StateMachineRun(&s);
    }
}
