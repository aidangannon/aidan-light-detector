#ifndef INTERRUPT_H
#define INTERRUPT_H

//
/**
 * quite restrictive but really don't want too many interrupts running on single vector
 */
#define MAX_INTERRUPTS 8

typedef void (*interrupt_handler_t)(void);

/**
 * very specific to old implementations of PIC microcontrollers, newer versions
 * have multiple interrupt vectors
 * @param number id of the interrupt (which also specifies its order in the array)
 * @param handler the function called when the ISR is run
 */
void register_interrupt_handler(
    char number,
    interrupt_handler_t handler
);

void delay_ms(const unsigned int ms);

#endif
