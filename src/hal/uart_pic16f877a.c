#include <hal_config.h>
#include <stdbool.h>
#define UART_ISR 1

/**
 * @brief Initialise the UART module for the PIC16F877A
 * - PORTC[6] becomes transmit pin for UART
 * - PORTC[7] becomes receive pin for UART
 * - 8 bit transmission with no parity
 */
void init_uart(
    const bool send_lock_config,
    const bool interrupt_enabled
) {
    // sets TX/TR ports
    TRISC6 = 1;
    TRISC7 = 1;

    // high baud rate & transmit enabled
    BRGH = 1;
    TXEN = 1;

    // continuous receive & serial port enabled
    CREN = 1;
    SPEN = 1;

    // baud rate value https://ww1.microchip.com/downloads/en/devicedoc/39582b.pdf TABLE 10-3:
    SPBRG = 25;

    // setting internal send and receive lock lines
    TRISC2 = send_lock_config;
    TRISC3 = !send_lock_config;

    // uart interrupt enable bit
    if (interrupt_enabled) {
        RCIE = 1;
    }
}

void uart_transmit(const char data) {
    TXREG=data;

    while((PIR1 & 0x10)==0);

    TXIF = 0;
}