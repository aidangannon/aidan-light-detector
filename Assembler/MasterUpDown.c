#include <system.h>

const int _XTAL_FREQ=4000000;//frequency of crystal resonator
const int INSTRUCTION_PERIOD=1;//time of clock speed in microseconds
const int PWM_PERIOD=20000; //the time period of PWM for servos in microseconds
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
char velocityPeriod=1; //ammount of time it takes to move 1 increment
char isUp;
char isDown;//used to accelerate the arm in a direction

char lock;

//--
//UART
//--
void initUART() 
{
    set_bit(trisc,6);
    set_bit(trisc,7);//setting trisc<6:7> as inputs
    
    txsta = 0x24;//enables high speed and write
    rcsta = 0x90;//enables trisc<6:7> ports and receive
    
    spbrg = 25;//timer
    
    set_bit(trisc,3);//status line send
    clear_bit(trisc,2);//status line listen
    
    set_bit(pie1,5);
    
}

void transUART(char data)
{
    txreg=data;
    while((pir1 & 0x10)==0);
    clear_bit(pir1,4);
}

char readUART()
{
    while((pir1 & 0x20)==0);
    clear_bit(pir1,5);
    return rcreg;
}

void sendStatusLine(char status)
{
    switch(status)
    {
        case 1:
            set_bit(portc,2);
            break;
        case 0:
            clear_bit(portc,2);
            break;
    }
}

char listenStatusLine(char status)
{
    switch(status)
    {
        case 1:
            if((portc & 0x08))
            {return 1;}
            else{return 0;}
            
        case 0:
            if((portc & 0x08)==0)
            {return 1;}
            else{return 0;}
    }
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


char UARTtimeOut(int x)
{
    while(x != 0)
    {   
        
        for(int z=1000;z!=0;z--){
           if((pir1 & 0x20)==0);
                clear_bit(pir1,5);
                return rcreg; 
        };// wait on overflow
        x--;
    }
    return 0;
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
    
    t1con=0x00; //sets value of prescaler to 8
    
    //setting value of compare register
    ccpr1h = 0xff;
    ccpr1l = 0xff;
    
    //trigger interupt when tmr1==compared value
    ccp1con=0x0B;
    
    //reseting tmr1
    tmr1h = 0;
    tmr1l = 0;
    
    clear_bit(pir1,2);
    
    //ENABLING TMR1 CCP INTERUPT
    set_bit(pie1,2); //enables the ccp interupt
    intcon = 0xC0; //enables peripheral interupts
    set_bit(t1con,0); //enables tmr1
	
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

char moveDown()
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
    ccpr1h = pwmLowPeriod>>8;
    ccpr1l = pwmLowPeriod;
}

void makeCCPHighPWMPeriod()
{
    //##
    //set ccp register to length of time (clock pulses)
    //-for low period of PWM
    //##
    
    ccpr1h = pwmHighPeriod>>8;
    ccpr1l = pwmHighPeriod;
    
}


//--
//LDRs
//--
void initLDR()
{

    //32 Fosc clock conversion
    //powering on ADC module
    adcon0=0x41;
    
    //all porta analog
    //adresh stores 8 bit value of ADC
    adcon1=0x00;
    
}

void getLDR(char value)
{
    //##
    //saves LDRs to respective values
    //##
    
    int ldrValue;
    clear_bit(adcon0,5);
    switch(value){
        case 0:
            //moves value 000 into CHS bit
            clear_bit(adcon0,4);
            clear_bit(adcon0,3);
            break;
        case 1:
            //moves value 001 into CHS bit
            clear_bit(adcon0,4);
            set_bit(adcon0,3);
            break;
        case 2:
            //moves value 010 into CHS bit
            set_bit(adcon0,4);
            clear_bit(adcon0,3);
            break;
        case 3:
            //moves value 011 into CHS bit
            set_bit(adcon0,4);
            set_bit(adcon0,3);
            break;
    }
    set_bit(adcon0,2);
    while(adcon0 & 0x04);
    ldrValue=adresh;
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
    
    //setting portb as output
    trisb=0;
    
    clear_bit(portc,2);//clearing send line
    
    initLDR();
    initPWMServos();
    
    //starting first pulse
    portb=0;
	set_bit(portb,1);
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
void interrupt(void)
{
    if((CCP1IF) != 0)
    {
        clear_bit(pir1,2);
        if(portb & 0x02){
            clear_bit(portb,1);
            makeCCPLowPWMPeriod();
		}else if(portb==0){
            set_bit(portb,1);
            makeCCPHighPWMPeriod();
		}
    }
    if((pir1 & 0x20)){
        clear_bit(pir1,5);
        positionSlave = rcreg;
    }
    
}