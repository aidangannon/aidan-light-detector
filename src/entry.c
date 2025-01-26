#include "pwm.h"
#include "uart.h"
#define FOREVER for(;;)

void main() {
    init_uart(true);

    const pwm_config servo_config = {
        .max_value = 192,
        .min_value = 40,
        .step_granularity = 12
    };

    init_pwm(&servo_config);

    FOREVER {
        // something
    }
}