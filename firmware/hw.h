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
#define ILLUMINATE          2,1
#define ILLUMINATE_ON()     HW_SET_HIGH(ILLUMINATE)
#define ILLUMINATE_OFF()    HW_SET_LOW(ILLUMINATE)
#define ILLUMINATE_TOGGLE() HW_SET_TOGGLE(ILLUMINATE)

#define INDICATE            2,2
#define INDICATE_ON()       HW_SET_LOW(INDICATE)
#define INDICATE_OFF()      HW_SET_HIGH(INDICATE)
#define INDICATE_TOGGLE()   HW_SET_TOGGLE(INDICATE)

#define LASER_DETECT_1      1,0
#define LASER_DETECT_1_ADC  0
#define LASER_DETECT_2      1,1
#define LASER_DETECT_2_ADC  1

// Interrupt
#define NUM_P1_INTS 2

// I2C
#define ADDRESS             0x0F
#define DAT                 1,7
#define CLK                 1,6

#endif
