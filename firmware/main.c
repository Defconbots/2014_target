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

static uint16_t ambient_red = 0;
static uint16_t ambient_green = 0;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//                             Hardware init
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void HwInit(void)
{
    // LEDs
    HW_OUTPUT(RED);
    RED_OFF();
    HW_OUTPUT(BLUE);
    BLUE_ON();

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
    uint8_t hit = (red > (ambient_red + 20)) && (green < (ambient_green + 50));
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
	while (glitch++ < 10)
	{
	    DumbDelay(1000);
		if (SET_READ())
		{
			return;
		}
	}
    StateMachinePublishEvent(&s,CONFIG);
}

void CntTick(void)
{
	uint8_t glitch = 0;
	while (glitch++ < 10)
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
void Idle(uint8_t ev)
{
    switch (ev)
    {
        case ENTER:
        {
            CallbackMode(CheckForHit,ENABLED);
            BLUE_ON();
            RED_OFF();
            Delay(1000);
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
            kill_count = 1;
            break;
        }
        case CNT_TICK:
        {
            kill_count++;
            uint8_t i = kill_count;
            while (i--)
            {
                RED_ON();
                Delay(50);
                RED_OFF();
                Delay(250);
            }
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
            Tcs3414Shutdown();
            kill_count--;
            break;
        }
        case IDLE:
        {
            Delay(3000);
        	StateMachinePublishEvent(&s, kill_count ? STUN_TIMEOUT : KILL);
        	break;
        }
        case EXIT:
        {
            HW_SET_HIGH(SET);
            HW_INPUT(SET);
            Tcs3414Init();
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
//                              Utilities
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void RecordAmbientLight(uint8_t num_samples)
{
    uint8_t i = 0;
    uint16_t red_sum = 0;
    uint16_t green_sum = 0;
    for (i = 0;i < num_samples;i++)
    {
        red_sum += Tcs3414ReadColor(COLOR_RED);
        green_sum += Tcs3414ReadColor(COLOR_GREEN);
        Delay(500);
    }
    ambient_red = red_sum / num_samples;
    ambient_green = green_sum / num_samples;
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
    InterruptAttach(SET,ConfigThrow,FALLING);
    InterruptAttach(CNT,CntTick,FALLING);
    _EINT();
    Delay(500);
    RecordAmbientLight(5);
    CallbackRegister(CheckForHit,50);
    while (1)
    {
        StateMachineRun(&s);
    }
}
