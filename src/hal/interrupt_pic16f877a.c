#include "hal_config.h"
#include "interrupt.h"

interrupt_handler_t handlers[MAX_INTERRUPTS] = {0};

void register_interrupt_handler(
    const char number,
    const interrupt_handler_t handler
) {
    if (number < MAX_INTERRUPTS) {
        handlers[number] = handler;
    }
}

void __interrupt() isr(void) {
    for (char i = 0; i < MAX_INTERRUPTS; i++) {
        if (handlers[i] != 0) {
            handlers[i]();
        }
    }
}