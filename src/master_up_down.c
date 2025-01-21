#define _XTAL_FREQ 4000000UL
#include <xc.h>

const int INSTRUCTION_PERIOD = 1;//time of clock speed in microseconds
const int PWM_PERIOD = 20000; //the time period of PWM for servos in microseconds
long pwmPeriod; //value that needs to be moved into
long pwmHighPeriod;
long pwmLowPeriod;

char ldr1;
char ldr2;
char ldr3;
char ldr4;

char upLdr;
char downLdr;

char positionBuff;
char positionBuffSlave;

char positionSlave;
char position;
char velocityPeriod = 1; //ammount of time it takes to move 1 increment
char isUp;
char isDown;//used to accelerate the arm in a direction

char lock;

//--
//UART
//--
void initUART() 
{
    TRISC |= (1 << 6);
    TRISC |= (1 << 7);//setting TRISC<6:7> as inputs

    TXSTA = 0x24;//enables high speed and write
    RCSTA = 0x90;//enables TRISC<6:7> ports and receive

    SPBRG = 25;//timer

    TRISC |= (1 << 3);//status line send
    TRISC &= ~(1 << 2);//status line listen

    PIE1 |= (1 << 5);

}

void transUART(char data)
{
    TXREG=data;
    while((PIR1 & 0x10)==0);
    PIR1 &= ~(1 << 4);
}

//--
//DELAY
//--
void delay(int x)
{
    while(x != 0)
    {
        for(int z=1000;z!=0;z--);// wait on overflow
        x--;
    }
}


//--
//SERVO
//--
void initPWMServos()
{
    //##
    //initialises everything
    //##

    //calculating pwm high & low periods
    pwmPeriod=PWM_PERIOD/INSTRUCTION_PERIOD;

    T1CON=0x00; //sets value of prescaler to 8

    //setting value of compare register
    CCPR1H = 0xff;
    CCPR1L = 0xff;

    //trigger interupt when tmr1==compared value
    CCP1CON=0x0B;

    //reseting tmr1
    TMR1H = 0;
    TMR1L = 0;

    PIR1 &= ~(1 << 2);

    //ENABLING TMR1 CCP INTERUPT
    PIE1 |= (1 << 2); //enables the ccp interupt
    INTCON = 0xC0; //enables peripheral interupts
    T1CON |= (1 << 0); //enables tmr1

}

char setDutyCycle(char multiplier)
{
    //##
    //sets duty cycle of the PWM signal for servos
    //##

    //range of the servos
    if(multiplier>192 || multiplier<40){
        return 0;
    }else{
        long highValue=12*multiplier;
        pwmHighPeriod=highValue;
        pwmLowPeriod=pwmPeriod-highValue;
        return 1;
    }
}

void moveUp()
{
    //accelerates the velocity
    if(isUp==1){
        velocityPeriod++;
    }else{
        velocityPeriod=1;
    }

    //the servo is left and not right
    isUp=1;
    isDown=0;

    if(setDutyCycle(position+velocityPeriod)==1){
        position+=velocityPeriod;
    }else{
        //no movement update
    }

}

void moveDown()
{
    //accelerates the velocity
    if(isDown==1){
        velocityPeriod++;
    }else{
        velocityPeriod=1;
    }

    //the servo is right and not left
    isUp=0;
    isDown=1;

    if(setDutyCycle(position-velocityPeriod)==1){
        position-=velocityPeriod;
    }else{
        //no movement
    }
}

void makeCCPLowPWMPeriod()
{
    //##
    //set ccp register to length of time (clock pulses)
    //-for high period of PWM
    //##

    //shifts 16 bit number into 2 8 bit registers
    CCPR1H = (unsigned char)pwmLowPeriod>>8;
    CCPR1L = (unsigned char)pwmLowPeriod;
}

void makeCCPHighPWMPeriod()
{
    //##
    //set ccp register to length of time (clock pulses)
    //-for low period of PWM
    //##

    CCPR1H = (unsigned char)pwmHighPeriod>>8;
    CCPR1L = (unsigned char)pwmHighPeriod;

}


//--
//LDRs
//--
void initLDR()
{

    //32 Fosc clock conversion
    //powering on ADC module
    ADCON0=0x41;

    //all porta analog
    //adresh stores 8 bit value of ADC
    ADCON1=0x00;

}

void getLDR(char value)
{
    //##
    //saves LDRs to respective values
    //##

    char ldrValue;
    ADCON0 &= ~(1 << 5);
    switch(value){
        case 0:
            //moves value 000 into CHS bit
            ADCON0 &= ~(1 << 4);
            ADCON0 &= ~(1 << 3);
            break;
        case 1:
            //moves value 001 into CHS bit
            ADCON0 &= ~(1 << 4);
            ADCON0 |= (1 << 3);
            break;
        case 2:
            //moves value 010 into CHS bit
            ADCON0 |= (1 << 4);
            ADCON0 &= ~(1 << 3);
            break;
        case 3:
            //moves value 011 into CHS bit
            ADCON0 |= (1 << 4);
            ADCON0 |= (1 << 3);
            break;
    }
    ADCON0 |= (1 << 2);
    while(ADCON0 & 0x04);
    ldrValue=ADRESH;
    switch(value){
        case 0:
            //moves value into ldr 1 variable
            ldr1=ldrValue;
            break;
        case 1:
            //moves value into ldr 2 variable
            ldr2=ldrValue;
            break;
        case 2:
            //moves value into ldr 3 variable
            ldr3=ldrValue;
            break;
        case 3:
            //moves value into ldr 4 variable
            ldr4=ldrValue;
            break;
    }

}

void getLDRs()
{
    //gets all ldr values and saves them
    //to public attributes
    getLDR(0);
    getLDR(1);
    getLDR(2);
    getLDR(3);
}

void avLDRs()
{
    //takes mean average of left and right ldrs
    downLdr=((ldr1+ldr4)/2);
    upLdr=((ldr2+ldr3)/2);
}


//--
//MAIN
//--
void main()
{

    //initialising UART
    initUART();

    //setting PORTB as output
    TRISB=0;

    PORTC &= ~(1 << 2);//clearing send line

    initLDR();
    initPWMServos();

    //starting first pulse
    PORTB=0;
    PORTB |= (1 << 1);
    position=112;
    setDutyCycle(position);
    delay(200);

    while(1){

        //gets the LDR values saves then upLDRs and downLDRs
        getLDRs();
        avLDRs();

        lock=0;


        //movement for left/right motor
        if(((downLdr-upLdr<14)&&(downLdr-upLdr>0))||((downLdr-upLdr>-14)&&(downLdr-upLdr<0))){
            if((positionBuff != position)||(positionBuffSlave !=positionSlave)){
                transUART(position);
                delay(1);
                transUART(positionSlave);
                delay(1);
                positionBuff = position;
                positionBuffSlave=positionSlave;
            }

        }
        else if(downLdr<upLdr){
            moveDown();
        }else if(upLdr<downLdr){
            moveUp();
        }

        delay(10);

    }
}


//--
//INTERUPT
//--
void __interrupt() interrupt(void)
{
    if((CCP1IF) != 0)
    {
        PIR1 &= ~(1 << 2);
        if(PORTB & 0x02){
            PORTB &= ~(1 << 1);
            makeCCPLowPWMPeriod();
        }else if(PORTB==0){
            PORTB |= (1 << 1);
            makeCCPHighPWMPeriod();
        }
    }
    if((PIR1 & 0x20)){
        PIR1 &= ~(1 << 5);
        positionSlave = RCREG;
    }

}
