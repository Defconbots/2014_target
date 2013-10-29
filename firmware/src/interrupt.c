/**
@file interrupt.c
@brief GPIO interrupt handler
@author Joe Brown
*/
#include "global.h"
#include "../hw.h"
#include "interrupt.h"

/** @brief table of function pointers attached to port1 GPIO pins*/
#ifdef NUM_P1_INTS
static InterruptFn p1_int_table[NUM_P1_INTS];
#endif
/** @brief table of function pointers attached to port2 GPIO pins*/
#ifdef NUM_P2_INTS
static InterruptFn p2_int_table[NUM_P2_INTS];
#endif

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
@brief Search interrupt function pointer lists and run functions corresponding
to the interrupt that fired
@details
Go through each bit in the interrupt flag to see if it is set. If it is run the
corresponding function.
@param[in] port port to search
*/
static void InterruptRunOnPort(uint8_t port);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Note: functions called from GPIO interrupts have no access to timer-based
// facilities (like the global "g_now" variable used for timing). The current
// Delay() implementation will also not work as it relies on timers.
void InterruptAttach(uint8_t port, uint8_t pin, InterruptFn func, enum IntEdgeType type)
{

#define ATTACH_CASE(x)                  \
    case x:                             \
        p##x##_int_table[pin] = func;   \
        P##x##IFG &= ~_BV(pin);         \
        if (type == FALLING)            \
        {                               \
            P##x##IES |= _BV(pin);      \
        }                               \
        else                            \
        {                               \
            P##x##IES &= ~_BV(pin);     \
        }                               \
        P##x##IE |= _BV(pin);           \
        break;

    switch (port)
    {
#ifdef NUM_P1_INTS
        ATTACH_CASE(1);
#endif
#ifdef NUM_P2_INTS
        ATTACH_CASE(2);
#endif
    }
#undef ATTACH_CASE
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void InterruptDetach(uint8_t port, uint8_t pin)
{

#define DETACH_CASE(x)                  \
    case x:                             \
        p##x##_int_table[pin] = NULL;   \
        P##x##IE  &= ~_BV(pin);         \
        P##x##IFG &= ~_BV(pin);         \
        break;

    switch (port)
    {
#ifdef NUM_P1_INTS
        DETACH_CASE(1);
#endif
#ifdef NUM_P2_INTS
        DETACH_CASE(2);
#endif
    }
#undef DETACH_CASE
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Any GPIO interrupt on any port triggers a single interrupt. We will look at
// PxIFG to determine which pin actually caused the interrupt and if an attached
// function is associated with it we will call the function.
void InterruptRunOnPort(uint8_t port)
{
#define PORT_CASE(x)                                                    \
    case x:                                                             \
        {                                                               \
            uint8_t i = 0;                                              \
            for (i = 0;i < NUM_P##x##_INTS;i++)                         \
            {                                                           \
                if ((P##x##IFG & _BV(i)) && p##x##_int_table[i] != NULL)\
                {                                                       \
                    p##x##_int_table[i]();                              \
                    P##x##IFG &= ~_BV(i);                               \
                }                                                       \
            }                                                           \
        }                                                               \
        break;

    _DINT();
    switch(port)
    {
#ifdef NUM_P1_INTS
        PORT_CASE(1);
#endif
#ifdef NUM_P2_INTS
        PORT_CASE(2);
#endif
    }
    _EINT();
#undef PORT_CASE
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//        ____        __                                   __
//       /  _/____   / /_ ___   _____ _____ __  __ ____   / /_ _____
//       / / / __ \ / __// _ \ / ___// ___// / / // __ \ / __// ___/
//     _/ / / / / // /_ /  __// /   / /   / /_/ // /_/ // /_ (__  )
//    /___//_/ /_/ \__/ \___//_/   /_/    \__,_// .___/ \__//____/
//                                             /_/
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#pragma vector = PORT1_VECTOR
__interrupt void Port1(void)
{
    InterruptRunOnPort(1);
}

#pragma vector = PORT2_VECTOR
__interrupt void Port2(void)
{
    InterruptRunOnPort(2);
}
