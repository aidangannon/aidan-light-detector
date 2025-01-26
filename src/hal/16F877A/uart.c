#include "../hal_config.h"
#include "../common.h"

#define UART_ISR 1
#define UART_TX_BUFFER_SIZE 64

typedef struct {
    char buffer[UART_TX_BUFFER_SIZE];
    volatile unsigned char index;
    bool transmitting;
} uart_tx_buffer_t;

static uart_tx_buffer_t tx_buffer;

bool tx_buffer_is_full() {
    return tx_buffer.index >= UART_TX_BUFFER_SIZE;
}

bool tx_buffer_is_empty() {
    return tx_buffer.index == 0;
}

char tx_buffer_dequeue() {
    if (tx_buffer_is_empty()) {
        return 0;
    }

    char data = tx_buffer.buffer[tx_buffer.index - 1];
    tx_buffer.index--;
    return data;
}

void handle_uart_interrupt(void) {
    if (TXIF && TXIE && tx_buffer.transmitting) {
        if (!tx_buffer_is_empty()) {
            TXREG = tx_buffer_dequeue();
        }
        else {
            tx_buffer.transmitting = false;
        }

        TXIF = false;
    }
}

bool tx_buffer_try_enqueue(const char data) {
    if (tx_buffer_is_full()) {
        return false;
    }
    tx_buffer.buffer[tx_buffer.index] = data;
    tx_buffer.index++;
    return true;
}

void init_uart(
    const bool send_lock_config
) {
    // sets TX/TR ports
    TRISC6 = true;
    TRISC7 = true;

    // high baud rate & transmit enabled
    BRGH = true;
    TXEN = true;

    // continuous receive & serial port enabled
    CREN = true;
    SPEN = true;

    // baud rate value https://ww1.microchip.com/downloads/en/devicedoc/39582b.pdf TABLE 10-3:
    SPBRG = 25;

    // setting internal send and receive lock lines
    TRISC2 = send_lock_config;
    TRISC3 = !send_lock_config;

    register_interrupt_handler(UART_ISR, handle_uart_interrupt);
}

bool uart_transmit(const char data) {
    // unset interrupts to prevent race conditions
    TXIE = 0;
    if (!tx_buffer_try_enqueue(data)) {
        return false;
    }

    if (!tx_buffer.transmitting) {
        tx_buffer.transmitting = true;
        TXREG = tx_buffer_dequeue();
    }

    // enable interrupts to allow data sending
    TXIE = 1;
    return true;
}