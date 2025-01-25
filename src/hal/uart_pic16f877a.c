#ifdef __XC8__

#include <xc.h>

void init_uart(void) {
    TRISC |= (1 << 6);
    TRISC |= (1 << 7);

    TXSTA = 0x24;
    RCSTA = 0x90;

    SPBRG = 25;

    TRISC |= (1 << 3);
    TRISC &= ~(1 << 2);

    PIE1 |= (1 << 5);
}

void uart_transmit(char data) {
    // do nothing
}

#endif