#pragma once

#ifndef __XC8__

extern volatile unsigned char TRISA;
extern volatile unsigned char TRISB;
extern volatile unsigned char TRISC;
extern volatile unsigned char PORTA;
extern volatile unsigned char PORTB;
extern volatile unsigned char PORTC;
extern volatile unsigned char TXSTA;
extern volatile unsigned char RCSTA;
extern volatile unsigned char SPBRG;
extern volatile unsigned char TXREG;
extern volatile unsigned char PIR1;
extern volatile unsigned char CCPR1H;
extern volatile unsigned char CCPR1L;
extern volatile unsigned char CCP1CON;
extern volatile unsigned char TMR1H;
extern volatile unsigned char TMR1L;
extern volatile unsigned char PIE1;
extern volatile unsigned char INTCON;
extern volatile unsigned char T1CON;
extern volatile unsigned char ADCON1;
extern volatile unsigned char ADCON0;
extern volatile unsigned char ADRESH;
extern volatile unsigned char CCP1IF;
extern volatile unsigned char RCREG;

#define __interrupt()

#pragma message "Compiling without XC8 and without __XC8__ is defined"

#endif

#ifdef __XC8__

#include <xc.h>
#pragma message "Compiling with XC8 and __XC8__ is defined"

#endif