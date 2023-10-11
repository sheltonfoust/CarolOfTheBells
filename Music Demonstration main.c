#include <msp430.h> 


/**
 * Travis Dickens
 *
 * Music Test Code
 *
 * plays a short array on repeat with three bells
 *
 * Next iterations:
 *  --Move timer to interrupts--
 *  Increase supported bells to 8
 *  --Make variable tempos--
 *  Add song selection
 */
extern int t = 0;
extern int change = 0;
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                   // Stop Watchdog timer
    PM5CTL0 &= ~LOCKLPM5;                       // Disable the GPIO power-on default high-impedance mode

    __enable_interrupt();                       // Enable interrupts

    //Set MCLK = SMCLK = 1MHz
    //BCSCTL1 = CALBC1_1MHZ;
    //DCOCTL = CALDCO_1MHZ;



    P2DIR |= BIT1;                              // Set P2.1 to output direction
    P1DIR |= BIT5;                              // Set P1.5 to output direction
    P9DIR |= BIT4;                              // Set P9.4 to output direction

    //Timer0_A3 Configuration
    TA0CCR0 = 65535; //Initially, Stop the Timer
    TA0CCTL0 |= CCIE; //Enable interrupt for CCR0.
    TA0CTL = TASSEL_2 + ID_3 + MC_1; //Select SMCLK, SMCLK/1, Up Mode

    int note[23] = {0,4,2,1,0,4,2,1,0,1,1,1,1,2,2,2,2,4,2,1,0,0,0};    //Music Array
    while(1){
        if(change == 1){
            change = 0;
            if(t==23){                   //Loops array
                t = 0;
            }
        if((note[t]&0b001)==0b001){         //Checks if the 1st bit is 1
            P2OUT |= BIT1;          //Trigger bell 1
        }
        if((note[t]&0b010)==0b010){    //Checks if the 2nd bit is 1
            P1OUT |= BIT5;          //Trigger bell 2
        }
        if((note[t]&0b100)==0b100){    //Checks if the 3rd bit is 1
            P9OUT |= BIT4;          //Trigger bell 3
        }
        __delay_cycles(10000);     //Delay for being "On"
        P2OUT &= ~BIT1;
        P1OUT &= ~BIT5;
        P9OUT &= ~BIT4;

        }
    }
}
#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
{
    t++; //Increment Over-Flow Counter
    change = 1;
}
