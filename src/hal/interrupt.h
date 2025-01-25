#ifndef INTERRUPT_H
#define INTERRUPT_H

// quite restrictive but works for now
#define MAX_INTERRUPTS 8

typedef void (*interrupt_handler_t)(void);

void register_interrupt_handler(
    char number,
    interrupt_handler_t handler
);

#endif
