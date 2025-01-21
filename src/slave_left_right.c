#define _XTAL_FREQ 4000000UL
#include <xc.h>

const int INSTRUCTION_PERIOD = 1; // time of clock speed in microseconds
const int PWM_PERIOD = 20000; // the time period of PWM for servos in microseconds
long pwmPeriod; // value that needs to be moved into
long pwmHighPeriod;
long pwmLowPeriod;

char ldr1;
char ldr2;
char ldr3;
char ldr4;

char leftLdr;
char rightLdr;

char positionBuff = 0;

char position;
char velocityPeriod = 1; // amount of time it takes to move 1 increment
char isLeft;
char isRight; // used to accelerate the arm in a direction

// values that determine if signal is locked
char lock;

//--
// UART
//--
void initUART()
{
    TRISC |= (1 << 6);
    TRISC |= (1 << 7); // setting TRISC<6:7> as inputs

    TXSTA = 0x24; // enables transmission and high speed
    RCSTA = 0x90; // enables TRISC<6:7> ports

    SPBRG = 25; // timer

    TRISC &= ~(1 << 3); // clear TRISC3 - status line send
    TRISC |= (1 << 2);  // set TRISC2 - status line listen
}

void transUART(char data)
{
    TXREG = data;
    while ((PIR1 & 0x10) == 0);
    PIR1 &= ~(1 << 4);
}

void sendStatusLine(char status)
{
    switch (status)
    {
        case 1:
            PORTC |= (1 << 3);
            break;
        case 0:
            PORTC &= ~(1 << 3);
            break;
    }
}

char listenStatusLine(char status)
{
    switch (status)
    {
        case 1:
            if ((PORTC & 0x04) == 0x04)
                return 1;
            else
                return 0;
        case 0:
            if ((PORTC & 0x04) == 0)
                return 1;
            else
                return 0;
    }
    return 0;
}

//--
// DELAY
//--
void delay(int x)
{
    while (x != 0)
    {
        for (int z = 1000; z != 0; z--); // wait on overflow
        x--;
    }
}

//--
// SERVO
//--
void initPWMServos()
{
    // ##
    // initializes everything
    // ##

    // calculating pwm high & low periods
    pwmPeriod = PWM_PERIOD / INSTRUCTION_PERIOD;

    T1CON = 0x00; // sets value of prescaler to 8

    // setting value of compare register
    CCPR1H = 0xFF;
    CCPR1L = 0xFF;

    // trigger interrupt when TMR1 == compared value
    CCP1CON = 0x0B;

    // resetting TMR1
    TMR1H = 0;
    TMR1L = 0;

    PIR1 &= ~(1 << 2);

    // ENABLING TMR1 CCP INTERRUPT
    PIE1 |= (1 << 2); // enables the CCP interrupt
    INTCON = 0xC0;    // enables peripheral interrupts
    T1CON |= (1 << 0); // enables TMR1
}

char setDutyCycle(char multiplier)
{
    // ##
    // sets duty cycle of the PWM signal for servos
    // ##

    // range of the servos
    if (multiplier > 192 || multiplier < 40)
    {
        return 0;
    }
    else
    {
        long highValue = 12 * multiplier;
        pwmHighPeriod = highValue;
        pwmLowPeriod = pwmPeriod - highValue;
        return 1;
    }
}

void moveLeft()
{
    // accelerates the velocity
    if (isLeft == 1)
    {
        velocityPeriod++;
    }
    else
    {
        velocityPeriod = 1;
    }

    // the servo is left and not right
    isLeft = 1;
    isRight = 0;

    if (setDutyCycle(position + velocityPeriod) == 1)
    {
        position += velocityPeriod;
    }
    else
    {
        // no movement update
    }
}

void moveRight()
{
    // accelerates the velocity
    if (isRight == 1)
    {
        velocityPeriod++;
    }
    else
    {
        velocityPeriod = 1;
    }

    // the servo is right and not left
    isLeft = 0;
    isRight = 1;

    if (setDutyCycle(position - velocityPeriod) == 1)
    {
        position -= velocityPeriod;
    }
    else
    {
        // no movement
    }
}

// sets ccp register to length of time (clock pulses) for low period of PWM
void makeCCPLowPWMPeriod()
{
    CCPR1H = (unsigned char)(pwmLowPeriod >> 8);
    CCPR1L = (unsigned char)pwmLowPeriod;
}

// sets ccp register to length of time (clock pulses) for high period of PWM
void makeCCPHighPWMPeriod()
{
    CCPR1H = (unsigned char)(pwmHighPeriod >> 8);
    CCPR1L = (unsigned char)pwmHighPeriod;
}

//--
// LDRs
//--
void initLDR()
{
    // 32 Fosc clock conversion
    // powering on ADC module
    ADCON0 = 0x41;

    // all PORTA analog
    // ADRESH stores 8 bit value of ADC
    ADCON1 = 0x00;
}

void getLDR(char value)
{
    // ##
    // saves LDRs to respective values
    // ##

    char ldrValue;
    ADCON0 &= ~(1 << 5);
    switch (value)
    {
        case 0:
            ADCON0 &= ~(1 << 4);
            ADCON0 &= ~(1 << 3);
            break;
        case 1:
            ADCON0 &= ~(1 << 4);
            ADCON0 |= (1 << 3);
            break;
        case 2:
            ADCON0 |= (1 << 4);
            ADCON0 &= ~(1 << 3);
            break;
        case 3:
            ADCON0 |= (1 << 4);
            ADCON0 |= (1 << 3);
            break;
    }
    ADCON0 |= (1 << 2);
    while (ADCON0 & 0x04)
        ;
    ldrValue = ADRESH;
    switch (value)
    {
        case 0:
            ldr1 = ldrValue;
            break;
        case 1:
            ldr2 = ldrValue;
            break;
        case 2:
            ldr3 = ldrValue;
            break;
        case 3:
            ldr4 = ldrValue;
            break;
    }
}

void getLDRs()
{
    // gets all ldr values and saves them to public attributes
    getLDR(0);
    getLDR(1);
    getLDR(2);
    getLDR(3);
}

void avLDRs()
{
    // takes mean average of left and right ldrs
    rightLdr = ((ldr2 + ldr4) / 2);
    leftLdr = ((ldr1 + ldr3) / 2);
}

//--
// MAIN
//--
void main()
{
    // initializing UART
    initUART();

    // setting PORTB as output
    TRISB = 0;

    initLDR();
    initPWMServos();

    PORTC &= ~(1 << 3); // clear send line

    // starting first pulse
    PORTB = 0;
    PORTB |= (1 << 1);
    position = 112;
    setDutyCycle(position);
    delay(200);

    while (1)
    {
        // gets the LDR values saves then upLDRs and downLDRs
        getLDRs();
        avLDRs();

        // movement for left/right motor
        if (((rightLdr - leftLdr < 14) && (rightLdr - leftLdr > 0)) || ((rightLdr - leftLdr > -14) && (rightLdr - leftLdr < 0)))
        {
            if (positionBuff != position)
            {
                transUART(position);
                positionBuff = position;
            }
        }
        else if (rightLdr < leftLdr)
        {
            moveLeft();
        }
        else if (leftLdr < rightLdr)
        {
            moveRight();
        }

        delay(10);
    }
}

//--
// INTERRUPT
//--
void __interrupt() interrupt(void)
{
    if (CCP1IF != 0)
    {
        PIR1 &= ~(1 << 2);
        if (PORTB & 0x02)
        {
            PORTB &= ~(1 << 1);
            makeCCPLowPWMPeriod();
        }
        else if (PORTB == 0)
        {
            PORTB |= (1 << 1);
            makeCCPHighPWMPeriod();
        }
        PIR1 &= ~(1 << 2);
    }
}
