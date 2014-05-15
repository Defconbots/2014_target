#include "global.h"
#include "hw.h"
#include "schedule.h"
#include "clock.h"
#include "delay.h"
#include "hardware_init.h"
#include "interrupt.h"
#include "state.h"
#include "tcs3414_color_sensor.h"
#include "juicy.h"

// One target needs pullups enabled for the set/cnt lines
//#define ENABLE_PULLUPS

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

void Detecting(uint8_t ev);
void Config(uint8_t ev);
void Stunned(uint8_t ev);
void Dead(uint8_t ev);


const Transition rules[] =
{
//  Current State + Event         = New State
    {Detecting,     STUN,           Stunned},
    {Detecting,     CONFIG,         Config},
    {Config,        CONFIG,         Detecting},
    {Config,        KILL,           Dead},
    {Stunned,       STUN_TIMEOUT,   Detecting},
    {Stunned,       KILL,           Dead},
    {Dead,          CONFIG,         Config}
};

static uint8_t kill_count = 0xFF;

static uint16_t ambient_red = 0;
static uint16_t ambient_green = 0;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//                              Utilities
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
    HW_INPUT(CNT);
#ifdef ENABLE_PULLUPS
    HW_PULL_UP(SET);
    HW_PULL_UP(CNT);
#endif
}

void RecordAmbientLight(uint8_t num_samples)
{
    uint8_t i = 0;
    uint16_t red_sum = 0;
    uint16_t green_sum = 0;
    Delay(100); // integration time for sensor
    for (i = 0;i < num_samples;i++)
    {
        red_sum += Tcs3414ReadColor(COLOR_RED);
        green_sum += Tcs3414ReadColor(COLOR_GREEN);
        Delay(100);
    }
    ambient_red = red_sum / num_samples;
    ambient_green = green_sum / num_samples;
}

void BroadcastHit(void)
{
    // Pull the set line low
    HW_OUTPUT(SET);
    HW_SET_LOW(SET);
}

void BroadcastReset(void)
{
    // Toggle high->low->high to force a CONFIG event on other targets
    HW_SET_HIGH(SET);
    Delay(500);
    HW_SET_LOW(SET);
    Delay(500);
    // Return to input/pullup state
    HW_SET_HIGH(SET);
    HW_INPUT(SET);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//                              Signal generators
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void CheckForHit(void)
{
    static uint32_t laser_hit_count = 0;
    uint16_t red   = Tcs3414ReadColor(COLOR_RED);
    uint16_t green = Tcs3414ReadColor(COLOR_GREEN);
    uint8_t hit = (red > (ambient_red + 100)) && (green < (ambient_green + 200));
    if (hit)
    {
        laser_hit_count++;
    }
    else
    {
        if (laser_hit_count >= 6 && laser_hit_count < 20) // 300-800ms
        {
            StateMachinePublishEvent(&s,STUN);
        }
        laser_hit_count = 0;
    }
}

void ConfigThrow(void)
{
    uint8_t glitch = 0;
    while (glitch++ < 100)
    {
        DumbDelay(1000);
        if (SET_READ())
        {
            return;
        }
    }
    StateMachinePublishEvent(&s, CONFIG);
}

void CntTick(void)
{
    uint8_t glitch = 0;
    while (glitch++ < 50)
    {
        DumbDelay(1000);
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
void Detecting(uint8_t ev)
{
    switch (ev)
    {
        case ENTER:
        {
            CallbackMode(CheckForHit,ENABLED);
            JuicyBlueOn();
            break;
        }
        case EXIT:
        {
            CallbackMode(CheckForHit,DISABLED);
            JuicyBlueOff();
            break;
        }
    }
}

void Config(uint8_t ev)
{
    static uint8_t cnt_state = 0;
    switch (ev)
    {
        case ENTER:
        {
            InterruptAttach(CNT,CntTick,FALLING);
            break;
        }
        case CNT_TICK:
        {
            if (cnt_state == 0)
            {
                cnt_state = 1;
                kill_count = 1;
            }
            else
            {
                kill_count++;
            }
            uint8_t i = kill_count;
            while (i--)
            {
                JuicyBothOn();
                JuicyBothOff();
                Delay(100);
            }
            break;
        }
        case EXIT:
        {
            InterruptDetach(CNT);
            cnt_state = 0;
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
            JuicyRedOn();
            BroadcastHit();
            Tcs3414Shutdown();
            kill_count--;
            break;
        }
        case IDLE:
        {
            Delay(1000);
            StateMachinePublishEvent(&s, kill_count ? STUN_TIMEOUT : KILL);
            break;
        }
        case EXIT:
        {
            InterruptDetach(SET);
            BroadcastReset();
            InterruptAttach(SET,ConfigThrow,FALLING);
            Tcs3414Init();
            JuicyRedOff();
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
            kill_count = 1;
            JuicyRedOn();
            Tcs3414Shutdown();
            break;
        }
        case EXIT:
        {
            JuicyRedOff();
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
    s = StateMachineCreate(rules,sizeof(rules), Detecting);
    InterruptAttach(SET,ConfigThrow,FALLING);
    _EINT();
    JuicyBlueOn();
    RecordAmbientLight(10);
    JuicyBlueOff();
    Delay(1000);
    CallbackRegister(CheckForHit,50);
    while (1)
    {
        StateMachineRun(&s);
    }
}
