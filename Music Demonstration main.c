#include <msp430.h> 


/**
 * Travis Dickens
 *
 * Music Test Code
 *
 * plays a short array on repeat with three bells
 *
 * Next iterations:
 *  Move timer to interrupts
 *  Increase supported bells to 8
 *  Make variable tempos
 *  Add song selection
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                   // Stop Watchdog timer
    PM5CTL0 &= ~LOCKLPM5;                       // Disable the GPIO power-on default high-impedance mode

    P2DIR |= BIT1;                              // Set P2.1 to output direction
    P1DIR |= BIT5;                              // Set P1.5 to output direction
    P9DIR |= BIT4;                              // Set P9.4 to output direction

    int note[6] = {1,3,7,5,2,5};    //Music Array
    int t = 0;
    while(1){
        if((note[t]%2)==1){         //Checks if the 1st bit is 1
            P2OUT |= BIT1;          //Trigger bell 1
        }
        if(((note[t]>>1)%2)==1){    //Checks if the 2nd bit is 1
            P1OUT |= BIT5;          //Trigger bell 2
        }
        if(((note[t]>>2)%2)==1){    //Checks if the 3rd bit is 1
            P9OUT |= BIT4;          //Trigger bell 3
        }
        __delay_cycles(200000);     //Delay for being "On"
        P2OUT &= ~BIT1;
        P1OUT &= ~BIT5;
        P9OUT &= ~BIT4;
        __delay_cycles(500000);     //Controls speed
        t++;                        //Increments time
        if(t==6){                   //Loops array
            t = 0;
        }
    }
}
