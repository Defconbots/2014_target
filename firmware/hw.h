/**
@file hw.h
@brief Definitions hardware config
@author Joe Brown
*/
#ifndef HW_H
#define HW_H
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//            __  __                  __
//           / / / /____ _ _____ ____/ /_      __ ____ _ _____ ___
//          / /_/ // __ `// ___// __  /| | /| / // __ `// ___// _ \
//         / __  // /_/ // /   / /_/ / | |/ |/ // /_/ // /   /  __/
//        /_/ /_/ \__,_//_/    \__,_/  |__/|__/ \__,_//_/    \___/
//
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// LEDs
#define RED             1,2
#define RED_ON()        HW_SET_HIGH(RED)
#define RED_OFF()       HW_SET_LOW(RED)
#define RED_TOGGLE()    HW_SET_TOGGLE(RED)

#define BLUE            1,3
#define BLUE_ON()       HW_SET_HIGH(BLUE)
#define BLUE_OFF()      HW_SET_LOW(BLUE)
#define BLUE_TOGGLE()   HW_SET_TOGGLE(BLUE)

// Inter-module comm.
#define CNT             1,0
#define SET             1,1

// Interrupt config
#define NUM_P1_INTS     2

// Light sensor
#define SYNC            1,4
#define SYNC_LOW()      HW_SET_LOW(SYNC)
#define INT             1,5

#define DAT             1,7
#define DAT_LOW()       HW_SET_LOW(DAT)
#define DAT_HIGH()      HW_SET_HIGH(DAT)
#define DAT_INPUT()     HW_INPUT(DAT)
#define DAT_OUTPUT()    HW_OUTPUT(DAT)
#define DAT_READ()      HW_READ(DAT)
#define CLK             1,6
#define CLK_LOW()       HW_SET_LOW(CLK)
#define CLK_HIGH()      HW_SET_HIGH(CLK)
#define CLK_INPUT()     HW_INPUT(CLK)
#define CLK_OUTPUT()    HW_OUTPUT(CLK)

#endif
