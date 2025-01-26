#ifndef PWM_H
#define PWM_H

/**
 * how many instructions per clock cycle
 */
#define INSTRUCTION_PERIOD 1

/**
 * period of the pwm module
 */
#define PWM_PERIOD 20000
#include "common.h"

const int SCALED_PWM_PERIOD = PWM_PERIOD / INSTRUCTION_PERIOD;

/**
 * step_granularity - describes how much to move the pwm over time
 */
typedef struct {
    int max_value;
    int min_value;
    int step_granularity;
} pwm_config;

static pwm_config* get_config(const pwm_config* new_config) {
    static pwm_config config = {0};
    static bool initialized = false;

    if (new_config && !initialized) {
        config = *new_config;
        initialized = true;
    }

    return &config;
}

void init_pwm(const pwm_config* config);

bool set_duty_cycle(const char value);

#endif
