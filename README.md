# Light Detector - Robotic Arm Controller

A sophisticated embedded system that controls a robotic arm to track light sources using dual PIC16F877A microcontrollers. The system features advanced PWM servo control, asynchronous UART communication, and a comprehensive HAL (Hardware Abstraction Layer) for both simulation and hardware targets.

## 🏗️ Architecture Overview

### Dual-MCU System Design
- **Master MCU**: Up/down axis control and system coordination
- **Slave MCU**: Left/right axis control and light sensor processing
- **UART Communication**: Asynchronous, interrupt-driven data exchange between MCUs
- **Light Sensors**: 4x LDR array for precise light source tracking

### Hardware Platform
- **Microcontroller**: PIC16F877A (8-bit, 20MHz)
- **Compiler**: Microchip XC8 v3.00
- **Programming**: PICkit 2/3 compatible
- **Servos**: PWM-controlled positioning system

## 🛠️ Build System

### CMakePresets.json Configuration
The project uses CMake presets for different compilation targets:

```bash
# IDE Development & Simulation (GCC)
cmake --preset=ide
cmake --build build-ide

# Hardware Compilation (XC8)
cmake --preset=xc8  
cmake --build build-xc8
```

**Build Targets:**
- **`ide`**: GCC compilation for IDE development and simulation testing
- **`xc8`**: XC8 compilation for hardware deployment

### Makefile XC8 Wrapper
The Makefile serves as an intelligent wrapper around the XC8 compiler:

```bash
# Direct XC8 compilation
make PROG_NAME=master PROG_DEFINES="-D__MASTER__ -D__MOVEMENTCONFIG__"
make PROG_NAME=slave PROG_DEFINES="-D__SLAVE__"

# Clean build artifacts
make clean
```

**Features:**
- Automatic XC8 toolchain detection (`/opt/microchip/xc8/v3.00/`)
- Conditional compilation flags for master/slave configurations  
- Integrated build artifact management
- Assembly generation for debugging analysis

## 📁 Code Structure

### Hardware Abstraction Layer (HAL)

The project implements a sophisticated HAL with dual compilation paths:

```
src/hal/
├── 16F877A/          # Hardware implementations (XC8)
│   ├── common.c      # Interrupt management & timing
│   ├── uart.c        # Hardware UART with async queue
│   └── pwm.c         # Timer1-based servo PWM control
├── sim/              # Simulation implementations (GCC)  
│   ├── common.c      # Simulated interrupt handling
│   ├── uart.c        # UART simulation with console I/O
│   └── pwm.c         # PWM simulation with timing analysis
├── common.h          # Shared function declarations
├── uart.h            # UART interface definitions
├── pwm.h             # PWM configuration structures
└── hal_config.h      # Conditional compilation configuration
```

### Conditional Compilation Strategy

The HAL uses `#ifdef __XC8__` to select between hardware and simulation:

```c
#ifdef __XC8__
    #include <xc.h>                    // Hardware registers
    #define PORTB1 PORTBbits.RB1       // XC8 bit syntax
#else
    extern volatile unsigned char PORTB_bits[8];  // Simulated registers
    #define PORTB1 PORTB_bits[1]       // Array-based simulation
#endif
```

## ⚡ Advanced Features

### PWM State Machine Architecture

The PWM system uses **CCP Compare Mode** with Timer1 for precise servo control:

**Why Compare Mode?**
- Hardware PWM limited to ~4ms max period (insufficient for 20ms servo requirement)
- Compare mode with 16-bit registers supports full 65ms range
- Enables precise 1-2ms pulse width control within 20ms periods

**State Machine Implementation:**
```c
typedef enum {
    SERVO_HIGH_PHASE,    // Outputting servo pulse (1-2ms)
    SERVO_LOW_PHASE      // Waiting for next period (18-19ms)
} servo_phase_t;

// Interrupt-driven state transitions
void handle_pwm_interrupt(void) {
    switch(servo_state) {
        case SERVO_LOW_PHASE:
            PORTB1 = 1;                    // Start pulse
            CCPR1 = pulse_width_ticks;     // Set pulse duration
            servo_state = SERVO_HIGH_PHASE;
            break;
        case SERVO_HIGH_PHASE:  
            PORTB1 = 0;                    // End pulse
            CCPR1 = period_ticks - pulse_width_ticks;  // Wait for next cycle
            servo_state = SERVO_LOW_PHASE;
            break;
    }
}
```

### Asynchronous UART with Interrupt-Driven Queue

**Lock-Free Circular Buffer Design:**
```c
typedef struct {
    char buffer[UART_TX_BUFFER_SIZE];
    volatile unsigned char head;    // Producer index (main code)
    volatile unsigned char tail;    // Consumer index (ISR)
} uart_tx_buffer_t;

// Thread-safe: single producer (main), single consumer (ISR)
bool uart_transmit(const char data) {
    // Atomic enqueue operation
    if (!buffer_full()) {
        buffer[head] = data;
        head = (head + 1) % BUFFER_SIZE;    // Atomic update
        
        if (!transmitting) {
            TXREG = buffer_dequeue();       // Start transmission
            transmitting = true;
        }
        return true;
    }
    return false;  // Buffer full
}
```

**Key Features:**
- **Non-blocking transmission**: `uart_transmit()` returns immediately
- **Automatic flow control**: ISR manages transmission without CPU intervention  
- **Buffer overflow protection**: Graceful handling of queue saturation
- **Race condition safe**: Single producer/consumer eliminates locks

### Interrupt Management System

**Function Pointer Registry:**
```c
typedef void (*interrupt_handler_t)(void);
static interrupt_handler_t handlers[MAX_INTERRUPTS] = {0};

void register_interrupt_handler(char number, interrupt_handler_t handler) {
    handlers[number] = handler;
}

void __interrupt() isr(void) {
    for (char i = 0; i < MAX_INTERRUPTS; i++) {
        if (handlers[i] != NULL) {
            handlers[i]();    // Dispatch to registered handler
        }
    }
}
```

**Benefits:**
- **Modular interrupt handling**: Each peripheral manages its own ISR
- **Dynamic registration**: Handlers can be registered at runtime
- **Clean separation**: Hardware ISR separate from application logic

## 🚀 Development Workflow

### Quick Start

1. **Clone and setup:**
   ```bash
   git clone <repository-url>
   cd aidan-light-detector
   ```

2. **IDE development (simulation):**
   ```bash
   cmake --preset=ide
   cmake --build build-ide
   ./build-ide/master    # Run master simulation
   ```

3. **Hardware compilation:**
   ```bash
   cmake --preset=xc8
   cmake --build build-xc8
   # Flash build-xc8/master.hex to hardware
   ```

### Build Artifacts

**Hardware Build Outputs:**
- `build/master/entry.hex` - Master MCU firmware
- `build/slave/entry.hex` - Slave MCU firmware  
- `build/*/entry.s` - Assembly listings for analysis

**Memory Usage Example:**
```
PIC16F877A Memory Summary:
Program space used: 555/8192 words (6.8%)
Data space used:    143/368 bytes  (38.9%)
```

### Testing Strategy

**Simulation Testing:**
- Unit tests run with GCC simulation targets
- Hardware register simulation for validation
- UART console I/O for debugging

**Hardware Validation:**
- Oscilloscope verification of PWM timing
- Logic analyzer for UART protocol analysis
- Real-time servo positioning feedback

## 📊 Technical Specifications

| Component | Specification |
|-----------|---------------|
| **MCU** | PIC16F877A @ 20MHz |
| **Compiler** | XC8 v3.00 |
| **PWM Frequency** | 50Hz (20ms period) |
| **PWM Resolution** | 16-bit (65536 steps) |
| **UART Buffer** | 64-byte circular queue |
| **Flash Usage** | ~6.8% (555/8192 words) |
| **RAM Usage** | ~38.9% (143/368 bytes) |

## 🔧 Configuration

### Master MCU Configuration
```c
#define __MASTER__
#define __MOVEMENTCONFIG__
// Handles up/down servo control and system coordination
```

### Slave MCU Configuration  
```c
#define __SLAVE__
// Handles left/right servo control and LDR sensor processing
```

### Hardware Connections
- **PWM Output**: PORTB,1 (servo control signal)
- **UART TX/RX**: PORTC,6/7 (inter-MCU communication)
- **LDR Inputs**: PORTA,0-3 (analog light sensors)
- **Status LEDs**: PORTB,0-7 (system status indication)

## 🤝 Contributing

This project demonstrates advanced embedded systems techniques including:
- Hardware abstraction layer design
- Interrupt-driven peripheral management  
- Real-time servo control algorithms
- Lock-free concurrent programming
- Cross-platform build system architecture
