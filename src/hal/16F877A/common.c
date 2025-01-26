#include "../hal_config.h"
#include "../common.h"

#include <stddef.h>
#define _XTAL_FREQ 4000000

static interrupt_descriptor* interrupt_descriptors[MAX_INTERRUPTS] = {0};

void register_interrupt_handler(
    const char number,
    interrupt_descriptor* handler
) {
    if (number < MAX_INTERRUPTS) {
        interrupt_descriptors[number] = handler;
    }
}

void delay_ms(const unsigned int ms) {
    for (unsigned int i = 0; i < ms; i++) {
        __delay_ms(1);
    }
}

/**
 * todo: probably a bit inefficient since we're doing a lot of memory lookups
 */
void __interrupt() isr(void) {
    for (char i = 0; i < MAX_INTERRUPTS; i++) {
        interrupt_descriptor* descriptor = interrupt_descriptors[i];
        if (descriptor->handler != NULL) {
            if (*descriptor->flag_reg & descriptor->flag_mask &&
                *descriptor->enable_reg & descriptor->enable_mask) {
                descriptor->handler();
            }

            if (descriptor->clear_type == SOFTWARE) {
                *descriptor->flag_reg &= ~descriptor->flag_mask;
            }
        }
    }
}