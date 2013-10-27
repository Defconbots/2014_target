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
#define ILLUMINATE_ON()     HW_SET(ILLUMINATE,_HIGH)
#define ILLUMINATE_OFF()    HW_SET(ILLUMINATE,_LOW)
#define ILLUMINATE_TOGGLE() HW_SET(ILLUMINATE,_TOGGLE)

#define INDICATE            2,2
#define INDICATE_ON()       HW_SET(INDICATE,_LOW)
#define INDICATE_OFF()      HW_SET(INDICATE,_HIGH)
#define INDICATE_TOGGLE()   HW_SET(INDICATE,_TOGGLE)

#define LASER_DETECT_1      1,0
#define LASER_DETECT_2      1,0

// Interrupt
#define NUM_P1_INTS 2

// I2C
#define ADDRESS             0x0F
#define DAT                 1,7
#define CLK                 1,6

#endif
