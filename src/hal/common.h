#ifndef INTERRUPT_H
#define INTERRUPT_H

#define _XTAL_FREQ 4000000

#define true 1
#define false 0

typedef unsigned char bool;

//
/**
 * quite restrictive but really don't want too many interrupts running on single vector
 */
#define MAX_INTERRUPTS 8

typedef void (*interrupt_handler_t)(void);

typedef enum {
    /**
     * hardware clears automatically
     */
    HARDWARE,
    /**
     * software clears manually
     */
    SOFTWARE
} flag_clear_type;

typedef struct {
    interrupt_handler_t handler;
    volatile unsigned char* flag_reg;
    unsigned char flag_mask;
    volatile unsigned char* enable_reg;
    unsigned char enable_mask;
    flag_clear_type clear_type;
} interrupt_descriptor;

/**
 * very specific to old implementations of PIC microcontrollers, newer versions
 * have multiple interrupt vectors
 * @param number id of the interrupt (which also specifies its order in the array)
 * @param handler the function called when the ISR is run
 */
void register_interrupt_handler(
    const char number,
    interrupt_descriptor* handler
);

void enable_global_interrupts(void);

#endif
