#ifndef HAL_CONFIG_H
#define HAL_CONFIG_H

#ifdef __XC8__

#include <xc.h>

#else

extern volatile unsigned char TRISC, TXSTA, RCSTA, SPBRG, PIE1, TXREG;

#define TRISC2 TRISC_bits[7]
#define TRISC3 TRISC_bits[6]
#define TRISC6 TRISC_bits[6]
#define TRISC7 TRISC_bits[7]
#define BRGH   TXSTA_bits[2]
#define TXEN   TXSTA_bits[5]
#define CREN   RCSTA_bits[4]
#define SPEN   RCSTA_bits[7]
#define TXIE   PIE1_bits[4]
#define TXIF   PIR1_bits[4]

extern volatile unsigned char TRISC_bits[8];
extern volatile unsigned char TXSTA_bits[8];
extern volatile unsigned char RCSTA_bits[8];
extern volatile unsigned char PIE1_bits[8];
extern volatile unsigned char PIR1_bits[8];

void __delay_ms(unsigned int ms);

#define __interrupt()

#endif

#endif
