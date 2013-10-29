/**
@file hardware_init.h
@brief Macros to init hardware
@author Joe Brown
*/
#ifndef HARDWARE_INIT_H
#define HARDWARE_INIT_H

// The macros below are an obvious abuse of the preprocessor. That said, when
// expanded they are just as efficient as writing a single-line C statement and
// will likely compile down to single-clock-cycle instructions.
// Example:
// #define LED 1,5
// HW_DIR(LED,_OUTPUT); expands to P1DIR |= (1<<(5));
// HW_DIR(LED,_INPUT);  expands to P1DIR &=~ (1<<(5));

#define OP_CLEAR                &=~
#define OP_SET                  |=
#define OP_TOGGLE               ^=

#define _INPUT                  OP_CLEAR
#define _OUTPUT                 OP_SET
/**@brief Set the direction of an IO pin as INPUT or OUTPUT */
#define HW_DIR(n,op)            HW_DIR_(n,op)
#define HW_DIR_(p,i,op)         P##p##DIR op _BV(i)
#define HW_INPUT(n)             HW_DIR_(n,_INPUT)
#define HW_OUTPUT(n)            HW_DIR_(n,_OUTPUT)

#define _IO                     OP_CLEAR
#define _SPECIAL                OP_SET
/**@brief Set function of IO pin */
#define HW_FUNC(n,op)           HW_FUNC_(n,op)
#define HW_FUNC_(p,i,op)        P##p##SEL op _BV(i)
#define HW_IO(n)                HW_FUNC_(n,_IO)
#define HW_SPECIAL(n)           HW_FUNC_(n,_SPECIAL)

/**@brief Set auxilary function of IO pin */
#define HW_AUX_FUN(n,op)        HW_AUX_FUN_(n,op)
#define HW_AUX_FUN_(p,i,op)     P##p##SEL2 op _BV(i)
#define HW_AUX_IO(n)            HW_AUX_FUN_(n,_IO);
#define HW_AUX_SPECIAL(n)       HW_AUX_FUN_(n,_SPECIAL);

#define _LOW                    OP_CLEAR
#define _HIGH                   OP_SET
#define _TOGGLE                 OP_TOGGLE
/**@brief Set state of IO pin */
#define HW_SET(n,op)            HW_SET_(n,op)
#define HW_SET_(p,i,op)         P##p##OUT op _BV(i)
#define HW_SET_LOW(n)           HW_SET_(n,_LOW)
#define HW_SET_HIGH(n)          HW_SET_(n,_HIGH)
#define HW_SET_TOGGLE(n)        HW_SET_(n,_TOGGLE)

#define _ENABLE                 OP_SET
#define _DISABLE                OP_CLEAR
/**@brief Set resistor pull up or down for IO pin */
#define HW_RES(n,op)            HW_RES_(n,op)
#define HW_RES_(p,i,op)         P##p##REN op _BV(i)

#define _PULL_DOWN              OP_CLEAR
#define _PULL_UP                OP_SET
// Set pin as input before calling this macro
/**@brief Set resistor polarity (up or down) for IO pin */
#define HW_PULL(n,op)           HW_RES_(n,_ENABLE);\
                                HW_SET_(n,op)
#define HW_PULL_UP(n)           HW_PULL_(n,_PULL_UP)
#define HW_PULL_DOWN(n)         HW_PULL_(n,_PULL_DOWN)

/**@brief Read in state of IO pin */
#define HW_READ(n)              HW_READ_(n)
#define HW_READ_(p,i)           (P##p##IN & _BV(i))

/**@brief Sent interrupt enable for IO pin */
#define HW_INT_EN(n,op)         HW_INT_EN_(n,op)
#define HW_INT_EN_(p,i,op)      P##p##IE op _BV(i)
#define HW_INT_ENABLE(n)        HW_INT_EN_(n,_ENABLE)
#define HW_INT_DISABLE(n)       HW_INT_EN_(n,_DISABLE)

#define _RISING_EDGE            OP_CLEAR
#define _FALLING_EDGE           OP_SET
/**@brief Set rising or falling edge interrupt for IO pin */
#define HW_INT_EDGE(n,op)       HW_INT_EDGE_(n,op)
#define HW_INT_EDGE_(p,i,op)    P##p##IES op _BV(i)
#define HW_INT_EDGE_FALLING(n)  HW_INT_EDGE_(n,_FALLING_EDGE)
#define HW_INT_EDGE_RISING(n)   HW_INT_EDGE_(n,_FALLING_EDGE)

#define _INT_SET                OP_SET
#define _INT_CLEAR              OP_CLEAR
/**@brief Set IO pin interrupt status */
#define HW_INT_FLAG(n,op)       HW_INT_FLAG_(n,op)
#define HW_INT_FLAG_(p,i,op)    P##p##IFG op _BV(i)
#define HW_INT_FLAG_SET(n)      HW_INT_FLAG_(n,_INT_SET);
#define HW_INT_FLAG_CLEAR(n)    HW_INT_FLAG_(n,_INT_CLEAR);

/**@brief Read IO pin interrupt status */
#define HW_INT_STAT(n)          HW_INT_STAT_(n)
#define HW_INT_STAT_(p,i)       (P##p##IFG & _BV(i))

#endif // HARDWARE_INIT_H
