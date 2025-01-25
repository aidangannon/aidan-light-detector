#include "hal_config.h"
#include "common.h"
#define _XTAL_FREQ 4000000

interrupt_handler_t handlers[MAX_INTERRUPTS] = {0};

void register_interrupt_handler(
    const char number,
    const interrupt_handler_t handler
) {
    if (number < MAX_INTERRUPTS) {
        handlers[number] = handler;
    }
}

void delay_ms(const unsigned int ms) {
    for (unsigned int i = 0; i < ms; i++) {
        __delay_ms(1);
    }
}

void __interrupt() isr(void) {
    for (char i = 0; i < MAX_INTERRUPTS; i++) {
        if (handlers[i] != 0) {
            handlers[i]();
        }
    }
}