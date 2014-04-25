/**
@file lib_config.h
@brief Definitions that change the settings of launchlib libraries
@author Joe Brown
*/
#ifndef CONFIG_H
#define CONFIG_H

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//           _____        __               __        __
//          / ___/ _____ / /_   ___   ____/ /__  __ / /___   _____
//          \__ \ / ___// __ \ / _ \ / __  // / / // // _ \ / ___/
//         ___/ // /__ / / / //  __// /_/ // /_/ // //  __// /
//        /____/ \___//_/ /_/ \___/ \__,_/ \__,_//_/ \___//_/
//
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// If you change the clock it will mess up the scheduler timing. To deal with
// this you can define ADJUST_SCHEDULER_ON_CLOCK_CONFIG. This will readjust
// the scheduler every time the clock is changed. If you never plan on changing
// the clock during runtime you do not need to enable this.
//#define ADJUST_SCHEDULER_ON_CLOCK_CONFIG
#define MAX_CALLBACK_CNT    1
#define MAX_CALLOUT_CNT     1

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//    _____  __          __           __  ___              __     _
//   / ___/ / /_ ____ _ / /_ ___     /  |/  /____ _ _____ / /_   (_)____   ___
//   \__ \ / __// __ `// __// _ \   / /|_/ // __ `// ___// __ \ / // __ \ / _ \
//  ___/ // /_ / /_/ // /_ /  __/  / /  / // /_/ // /__ / / / // // / / //  __/
// /____/ \__/ \__,_/ \__/ \___/  /_/  /_/ \__,_/ \___//_/ /_//_//_/ /_/ \___/
//
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#define MAX_EVENT_CNT       10

#endif
