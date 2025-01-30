#ifndef UART_H
#define UART_H
#include "common.h"

/**
 * @brief Initialise the UART module
 * @param send_lock_config: sets the lock config for two devices sending on the UART bus
 */
void init_uart(
    const bool send_lock_config
);
bool uart_transmit(const char data);
void handle_uart_interrupt(void);

#endif
