# Light Detector - Robotic Arm Controller

Dual PIC16F877A system for tracking close light with servos.

## Code

### PWM State Machine

Compare mode (the hard way) because hardware PWM is too friggin short man...
```c
typedef void (*state_handler)(pwm_controller* ctrl);

void action_high(pwm_controller* ctrl) {
    PORTB1 = false;
    CCPR1 = ctrl->low_period;
    ctrl->state = LOW;
}

static const state_transition state_table[PWM_STATE_COUNT] = {
    [HIGH] = { .state = LOW, .handler = action_high },
    [LOW]  = { .state = HIGH, .handler = action_low }
};

void handle_interrupt() {
    state_table[config->state].handler(config);
}
```

### UART Ring Buffer

Lock-free, say what...
```c
typedef struct {
    char buffer[UART_TX_BUFFER_SIZE];
    volatile unsigned char head;
    volatile unsigned char tail;
} uart_tx_circular_buffer;

bool tx_buffer_try_enqueue(const char data) {
    if (tx_buffer_is_full()) return false;
    tx_ring_buffer.buffer[tx_ring_buffer.head] = data;
    tx_ring_buffer.head = (tx_ring_buffer.head + 1) % UART_TX_BUFFER_SIZE;
    return true;
}
```

### Interrupt Descriptors
```c
typedef struct {
    interrupt_handler_t handler;
    volatile unsigned char* flag_reg;
    unsigned char flag_mask;
    flag_clear_type clear_type;
} interrupt_descriptor;

void __interrupt() isr(void) {
    if (*descriptor->flag_reg & descriptor->flag_mask) {
        descriptor->handler();
    }
}
```

## Build
```bash
cmake --preset=xc8 && cmake --build build-xc8
```

## Stack

PIC16F877A @ 20MHz • XC8 • 555 words of program space
