#ifndef UART_H
#define UART_H
#include <stdbool.h>

void init_uart(
    bool send_lock_config,
    bool interrupt_enabled
);
void uart_transmit(char data);
void handle_uart_interrupt(void);

#endif
