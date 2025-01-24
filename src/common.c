#include "xc8_shim.h"
#include "common.h"
#define _XTAL_FREQ 4000000

// Define constants
const int INSTRUCTION_PERIOD = 1;
const int PWM_PERIOD = 20000;

extern long pwmPeriod;
extern long pwmHighPeriod;
extern long pwmLowPeriod;
extern char ldr1, ldr2, ldr3, ldr4;

// UART functions are device-specific - kept in master_up_down.c and slave_left_right.c

// All functions are device-specific - keeping only constants in common