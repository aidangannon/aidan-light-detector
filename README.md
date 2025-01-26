# Light Detector - Robotic Arm Controller

Dual PIC16F877A microcontroller system for light-tracking robotic arm control with asynchronous UART communication and servo PWM control.

## Architecture

- **Master MCU**: Up/down axis control and coordination
- **Slave MCU**: Left/right axis control and LDR sensor processing  
- **Hardware**: PIC16F877A @ 20MHz, XC8 v3.00 compiler

## Build System

### CMake Presets
```bash
# Simulation (GCC)
cmake --preset=ide && cmake --build build-ide

# Hardware (XC8) 
cmake --preset=xc8 && cmake --build build-xc8
```

### Direct XC8 Compilation
```bash
make PROG_NAME=master PROG_DEFINES="-D__MASTER__ -D__MOVEMENTCONFIG__"
make PROG_NAME=slave PROG_DEFINES="-D__SLAVE__"
```

### CI/CD Pipeline
GitHub Actions automatically:
- Downloads XC8 compiler from repository releases
- Installs XC8 in unattended mode
- Builds firmware using CMake presets

## Code Structure

### Hardware Abstraction Layer
```
src/hal/
├── 16F877A/          # XC8 hardware implementations
├── sim/              # GCC simulation implementations  
├── hal_config.h      # Conditional compilation (#ifdef __XC8__)
└── *.h               # Shared interfaces
```

### Conditional Compilation
```c
#ifdef __XC8__
    #define PORTB1 PORTBbits.RB1     // XC8 hardware
#else
    #define PORTB1 PORTB_bits[1]     // Simulation
#endif
```

## PWM State Machine

Table-driven state machine using CCP Compare Mode for 20ms servo periods:

```c
typedef enum { LOW, HIGH } pwm_state;

typedef void (*state_handler)(pwm_controller* ctrl);

typedef struct {
    pwm_state state;
    state_handler handler;
} state_transition;

void action_high(pwm_controller* ctrl) {
    PORTB1 = false;
    CCPR1 = ctrl->low_period;    // 18-19ms wait period
    ctrl->state = LOW;
}

void action_low(pwm_controller* ctrl) {
    PORTB1 = true;
    CCPR1 = ctrl->high_period;   // 1-2ms servo pulse
    ctrl->state = HIGH;
}

static const state_transition state_table[PWM_STATE_COUNT] = {
    [HIGH] = { .state = LOW, .handler = action_high },
    [LOW]  = { .state = HIGH, .handler = action_low }
};

void handle_interrupt() {
    if (CCP1IF) {
        pwm_controller* config = get_pwm_controller(NULL);
        CCP1IF = false;
        state_table[config->state].handler(config);  // Execute state action
    }
}
```

**Why Compare Mode**: Hardware PWM limited to ~4ms periods; servos need 20ms.
**State Machine**: Clean separation of states with function pointer dispatch.

## UART Ring Buffer

Lock-free circular buffer for interrupt-driven transmission:

```c
typedef struct {
    char buffer[UART_TX_BUFFER_SIZE];
    volatile unsigned char head;     // Write pointer (producer)
    volatile unsigned char tail;     // Read pointer (consumer) 
    bool transmitting;
} uart_tx_circular_buffer;

bool tx_buffer_is_full() {
    return (tx_ring_buffer.head + 1) % UART_TX_BUFFER_SIZE == tx_ring_buffer.tail;
}

bool tx_buffer_try_enqueue(const char data) {
    if (tx_buffer_is_full()) return false;
    
    tx_ring_buffer.buffer[tx_ring_buffer.head] = data;  // Write at head
    tx_ring_buffer.head = (tx_ring_buffer.head + 1) % UART_TX_BUFFER_SIZE;  // Wrap around
    return true;
}

char tx_buffer_dequeue() {
    if (tx_buffer_is_empty()) return 0;
    
    char data = tx_ring_buffer.buffer[tx_ring_buffer.tail];  // Read from tail
    tx_ring_buffer.tail = (tx_ring_buffer.tail + 1) % UART_TX_BUFFER_SIZE;  // Wrap around
    return data;
}

void handle_uart_interrupt(void) {
    if (TXIF && TXIE && tx_ring_buffer.transmitting) {
        if (!tx_buffer_is_empty()) {
            TXREG = tx_buffer_dequeue();  // Send next byte
        } else {
            tx_ring_buffer.transmitting = false;  // Stop transmission
        }
        TXIF = false;
    }
}
```

**Ring Buffer Benefits**: FIFO ordering, efficient memory reuse, lock-free operation.

## Interrupt Management

Descriptor-based interrupt registration system with automatic flag checking and clearing:

```c
typedef struct {
    interrupt_handler_t handler;        // ISR function pointer
    volatile unsigned char* flag_reg;   // Interrupt flag register (PIR1, etc.)
    unsigned char flag_mask;            // Bit mask for specific interrupt
    volatile unsigned char* enable_reg; // Interrupt enable register (PIE1, etc.)
    unsigned char enable_mask;          // Bit mask for interrupt enable
    flag_clear_type clear_type;         // HARDWARE or SOFTWARE flag clearing
} interrupt_descriptor;

static interrupt_descriptor* interrupt_descriptors[MAX_INTERRUPTS] = {0};

void register_interrupt_handler(const char number, interrupt_descriptor* handler) {
    if (number < MAX_INTERRUPTS) {
        interrupt_descriptors[number] = handler;
    }
}

void __interrupt() isr(void) {
    for (char i = 0; i < MAX_INTERRUPTS; i++) {
        interrupt_descriptor* descriptor = interrupt_descriptors[i];
        if (descriptor->handler != NULL) {
            // Check both flag and enable bits
            if (*descriptor->flag_reg & descriptor->flag_mask &&
                *descriptor->enable_reg & descriptor->enable_mask) {
                descriptor->handler();
            }
            
            // Clear flag if software clearing required
            if (descriptor->clear_type == SOFTWARE) {
                *descriptor->flag_reg &= ~descriptor->flag_mask;
            }
        }
    }
}
```

### Module Registration Examples

**PWM Module** (Timer1 Compare interrupt):
```c
void init_pwm(const pwm_config* config) {
    // Hardware setup...
    CCP1CON = 0x0B;    // Compare mode
    CCP1IE = true;     // Enable CCP1 interrupt
    
    // Register interrupt handler
    interrupt_descriptor interrupt_config = {
        .handler = handle_interrupt,
        .enable_reg = &PIE1,
        .flag_reg = &PIR1, 
        .enable_mask = 0x04,        // CCP1IE bit
        .flag_mask = 0x04,          // CCP1IF bit
        .clear_type = SOFTWARE
    };
    register_interrupt_handler(PWM_ISR, &interrupt_config);
}
```

**UART Module** (Transmit interrupt):
```c
void init_uart(const bool send_lock_config) {
    // Hardware setup...
    TXIE = true;       // Enable TX interrupt
    
    // Register interrupt handler
    interrupt_descriptor interrupt_config = {
        .handler = handle_uart_interrupt,
        .enable_reg = &PIE1,
        .flag_reg = &PIR1,
        .enable_mask = 0x10,        // TXIE bit  
        .flag_mask = 0x10,          // TXIF bit
        .clear_type = HARDWARE      // TXIF cleared automatically
    };
    register_interrupt_handler(UART_ISR, &interrupt_config);
}
```

**Benefits**: Automatic flag/enable checking, configurable flag clearing, modular registration by interrupt ID.

## Memory Usage

```
PIC16F877A Memory Summary:
Program space: 555/8192 words (6.8%)
Data space:    143/368 bytes  (38.9%)
```

## Quick Start

1. Clone repository
2. For simulation: `cmake --preset=ide && cmake --build build-ide`
3. For hardware: `cmake --preset=xc8 && cmake --build build-xc8`
4. Flash `build/master/entry.hex` and `build/slave/entry.hex` to respective MCUs