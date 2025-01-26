#ifndef HAL_CONFIG_H
#define HAL_CONFIG_H

#ifdef __XC8__

#define PORTB1 PORTBbits.RB1

#include <xc.h>

#else

extern volatile unsigned char
TRISC,
TXSTA,
RCSTA,
SPBRG,
PIE1,
TXREG,
T1CON,
CCPR1H,
CCPR1L,
CCP1CON,
TMR1H,
TMR1L;

#define TRISB1 TRISB_bits[1]
#define TRISC2 TRISC_bits[2]
#define TRISC3 TRISC_bits[3]
#define TRISC6 TRISC_bits[6]
#define TRISC7 TRISC_bits[7]
#define PORTB1 PORTB_bits[1]
#define BRGH   TXSTA_bits[2]
#define TXEN   TXSTA_bits[5]
#define CREN   RCSTA_bits[4]
#define SPEN   RCSTA_bits[7]
#define TXIE   PIE1_bits[4]
#define TXIF   PIR1_bits[4]
#define CCP1IF PIR1_bits[2]
#define CCP1IE PIE1_bits[2]
#define PEIE INTCON_bits[6]
#define GIE INTCON_bits[7]
#define TMR1ON T1CON_bits[0]

extern volatile unsigned char TRISB_bits[8];
extern volatile unsigned char TRISC_bits[8];
extern volatile unsigned char PORTB_bits[8];
extern volatile unsigned char TXSTA_bits[8];
extern volatile unsigned char RCSTA_bits[8];
extern volatile unsigned char PIE1_bits[8];
extern volatile unsigned char PIR1_bits[8];
extern volatile unsigned char INTCON_bits[8];
extern volatile unsigned char T1CON_bits[8];
extern volatile unsigned int TMR1;
extern volatile unsigned int CCPR1;

void __delay_ms(unsigned int ms);

#define __interrupt()

#endif

#endif
