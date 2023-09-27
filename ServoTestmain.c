#include <msp430.h>
/*
int main(void) {
    //int degree = 10000;
    PM5CTL0 &= ~LOCKLPM5;
    WDTCTL = WDTPW + WDTHOLD; //Disable the Watchdog timer for our convenience.
        P1DIR |= BIT6; //Set pin 1.2 to the output direction.
        P1SEL0 |= BIT6; //Select pin 1.2 as our PWM output.
        P1SEL1 |= BIT6; //Select pin 1.2 as our PWM output.
        TA0CCR0 = 20000; //Set the period in the Timer A0 Capture/Compare 0 register to 1000 us.
        TA0CCTL1 = OUTMOD_7;
        TA0CCR1 = 10000; //The period in microseconds that the power is ON. It's half the time, which translates to a 50% duty cycle.
        TA0CTL = TASSEL_2 + MC_1; //TASSEL_2 selects SMCLK as the clock source, and MC_1 tells it to count up to the value in TA0CCR0.
        __bis_SR_register(LPM0_bits); //Switch to low power mode 0.
}
 */
int main(void) {
    //int degree = 10000;
    PM5CTL0 &= ~LOCKLPM5;
    WDTCTL = WDTPW + WDTHOLD; //Disable the Watchdog timer for our convenience.
    P1REN |= BIT1;                          // Enable Resistor on P1.1, Button 1

      P1OUT |= BIT1;                           // Enable Pull-up on P1.1, Button 1
        P2DIR |= BIT5; //Set pin 1.2 to the output direction.
        P2SEL0 |= BIT5; //Select pin 1.2 as our PWM output.
        //P2SEL1 |= BIT5; //Select pin 1.2 as our PWM output.
        TB0CCR0 = 25000; //Set the period in the Timer A0 Capture/Compare 0 register to 1000 us.
        TB0CCTL4 = OUTMOD_7;
        TB0CCR4 = 0; //The period in microseconds that the power is ON. It's half the time, which translates to a 50% duty cycle.
        TB0CTL = TBSSEL_2 + MC_1; //TASSEL_2 selects SMCLK as the clock source, and MC_1 tells it to count up to the value in TA0CCR0.
        while(1){
            /*
        if (!(P1IN & BIT1))               // If Button 1 is pressed
               {
            TB0CCR4 = 1200; //The period in microseconds that the power is ON. It's half the time, which translates to a 50% duty cycle.
               }
            else{
            TB0CCR4 = 1000; //The period in microseconds that the power is ON. It's half the time, which translates to a 50% duty cycle.
               }
             */
            TB0CCR4 = 1200;
            __delay_cycles(500000);
            TB0CCR4 = 1000;
            __delay_cycles(500000);
        }
        __bis_SR_register(LPM0_bits); //Switch to low power mode 0.
}
