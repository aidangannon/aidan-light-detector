#include "../hal_config.h"
#include "../common.h"

#define UART_ISR 1
#define UART_TX_BUFFER_SIZE 8

typedef struct {
    char buffer[UART_TX_BUFFER_SIZE];
    unsigned char head;
    unsigned char tail;
    unsigned char count;
} uart_tx_circular_buffer;

static bool transmitting = false;
static uart_tx_circular_buffer tx_ring_buffer = {0};

char tx_buffer_dequeue() {
    if (tx_ring_buffer.count == 0) {
        return false;
    }

    char data = tx_ring_buffer.buffer[tx_ring_buffer.tail];
    tx_ring_buffer.tail = (tx_ring_buffer.tail + 1) % UART_TX_BUFFER_SIZE;
    --tx_ring_buffer.count;
    return data;
}

bool tx_buffer_try_enqueue(const char data) {

    if (tx_ring_buffer.count == UART_TX_BUFFER_SIZE) {
        return false;
    }

    tx_ring_buffer.buffer[tx_ring_buffer.head] = data;
    tx_ring_buffer.head = (tx_ring_buffer.head + 1) % UART_TX_BUFFER_SIZE;
    ++tx_ring_buffer.count;
    return true;
}

void handle_uart_interrupt(void) {
    if (transmitting) {
        const unsigned char dequeue_result = tx_buffer_dequeue();
        if (dequeue_result == false) {
            transmitting = false;
            return;
        }

        TXREG = dequeue_result;
    }
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

    // enable interrupt for transmit
    TXIE = true;
    interrupt_descriptor interrupt_config = {
        .handler = handle_uart_interrupt,
        .enable_reg = &PIE1,
        .flag_reg = &PIR1,
        .enable_mask = 0x10,
        .flag_mask = 0x10
    };
    register_interrupt_handler(UART_ISR, &interrupt_config);
}

/**
 * - we clear GIE before transmit and re-enable after since there are multiple state increments
 * occurring on a global ring buffer, this is essentially a lock to prevent the code from jumping
 * to ISR half-way through a transmission
 * - not allowed null characters
 */
bool uart_transmit(const char data) {
    GIE = false;

    if (data == 0x00) {
        return false;
    }

    if (!tx_buffer_try_enqueue(data)) {
        return false;
    }

    if (transmitting == false) {
        TXREG = tx_buffer_dequeue();
        transmitting = true;
    }

    GIE = true;
    return true;
}