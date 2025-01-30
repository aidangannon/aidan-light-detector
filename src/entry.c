#include "hal_config.h"
#include "pwm.h"
#include "uart.h"

void main() {
    init_uart(true);

    const pwm_config servo_config = {
        .max_value = 192,
        .min_value = 40,
        .step_granularity = 12
    };

    init_pwm(&servo_config);

    enable_global_interrupts();

    FOREVER {
        // ReSharper disable once CppDFAEndlessLoop
        uart_transmit('H');
        __delay_ms(5000);
        uart_transmit('E');
        uart_transmit('L');
    }
}