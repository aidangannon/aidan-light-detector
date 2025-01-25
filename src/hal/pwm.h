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

void init_pwm(void);

#endif
