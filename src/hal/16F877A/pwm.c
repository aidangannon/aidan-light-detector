#include <stddef.h>
#include "../pwm.h"

#include "common.h"
#include "hal_config.h"

#define PWM_ISR 0

#define PWM_STATE_COUNT 2

typedef enum {
    LOW,
    HIGH
} pwm_state;

typedef struct {
    unsigned short current_value;
    unsigned short high_period;
    unsigned short low_period;
    pwm_state state;
} pwm_controller;

typedef void (*state_handler)(pwm_controller* ctrl);

typedef struct {
    pwm_state state;
    state_handler handler;
} state_transition;

void action_high(pwm_controller* ctrl) {
    PORTB1 = false;
    CCPR1 = ctrl->low_period;
    ctrl->state = LOW;
}

void action_low(pwm_controller* ctrl) {
    PORTB1 = true;
    CCPR1 = ctrl->high_period;
    ctrl->state = HIGH;
}

static const state_transition state_table[PWM_STATE_COUNT] = {
    [HIGH] = { .state = LOW, .handler = action_high },
    [LOW] = { .state = HIGH,  .handler = action_low }
};

static pwm_controller* get_pwm_controller(const pwm_controller* controller_init) {
    static pwm_controller controller = {0};
    static bool initialized = false;

    if (controller_init && !initialized) {
        controller = *controller_init;
        initialized = true;
    }

    return &controller;
}

void handle_interrupt() {
    pwm_controller* config = get_pwm_controller(NULL);
    state_table[config->state].handler(config);
}

void init_pwm(const pwm_config* config) {
    get_config(config);
    pwm_controller controller = {0};
    get_pwm_controller(&controller);

    // set to 1:1 prescale
    T1CON = T1CON & 0xCF | 0x00;

    // sets max value in the compares registers for TMR1, to not trigger interrupts straight away
    // will be overridden later on
    CCPR1 = 0xffff;

    // trigger interrupt when tmr1==compared value
    CCP1CON=0x0B;

    // resetting tmr1
    TMR1 = 0x0000;

    // enable compare mode for tmr1
    CCP1IF = true;

    // enables the ccp interrupt
    CCP1IE = true;

    // todo: probably need to move some global setup elsewhere
    // enables global interrupts
    GIE = true;

    // enables peripheral interrupts
    PEIE = true;

    // enables tmr1
    TMR1ON = true;

    // sets portb 1 as output pin
    TRISB1 = false;

    interrupt_descriptor interrupt_config = {
        .handler = handle_interrupt,
        .enable_reg = &PIE1,
        .flag_reg = &PIR1,
        .enable_mask = 0x04,
        .flag_mask = 0x04,
        .clear_type = SOFTWARE
    };
    register_interrupt_handler(PWM_ISR, &interrupt_config);
}

bool set_duty_cycle(const char value) {
    pwm_config config = *get_config(NULL);

    if (value < config.min_value || value > config.max_value) {
        return 0;
    }

    pwm_controller controller = *get_pwm_controller(NULL);

    controller.high_period = value * config.step_granularity;
    controller.low_period = PWM_PERIOD - controller.high_period;

    return true;
}