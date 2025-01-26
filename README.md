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

## PWM Implementation

Uses CCP Compare Mode (not hardware PWM) for 20ms servo periods:

```c
typedef enum { LOW, HIGH } pwm_state;

void handle_interrupt() {
    if (CCP1IF) {
        CCP1IF = false;
        if (PORTB1) {
            PORTB1 = false;
            CCPR1 = config.low_period;   // 18-19ms wait
        } else {
            PORTB1 = true;
            CCPR1 = config.high_period;  // 1-2ms pulse
        }
    }
}
```

**Why Compare Mode**: Hardware PWM limited to ~4ms periods; servos need 20ms.

## UART Queue System

Interrupt-driven transmission with circular buffer:

```c
typedef struct {
    char buffer[UART_TX_BUFFER_SIZE];
    volatile unsigned char index;
    bool transmitting;
} uart_tx_buffer_t;

void handle_uart_interrupt(void) {
    if (TXIF && TXIE && tx_buffer.transmitting) {
        if (!tx_buffer_is_empty()) {
            TXREG = tx_buffer_dequeue();
        } else {
            tx_buffer.transmitting = false;
        }
        TXIF = false;
    }
}
```

## Interrupt Management

Function pointer registry for modular interrupt handling:

```c
static interrupt_handler_t handlers[MAX_INTERRUPTS] = {0};

void __interrupt() isr(void) {
    for (char i = 0; i < MAX_INTERRUPTS; i++) {
        if (handlers[i] != NULL) {
            handlers[i]();
        }
    }
}
```

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