#include <msp430.h> 
#include <stdint.h>

#define SIG_LEN 7500
#define ROLL_PERIOD 7500


#define CAROL_LEN 8
#define CAROL_SHORT 8 // shortest note
#define CAROL_TEMPO 120 // Will be slightly incorrect
#define CAROL_PERIOD 1000000 / 8 * 60 * 4 / CAROL_TEMPO / CAROL_SHORT
#define CAROL_ROLLS_PER_NOTE CAROL_PERIOD / ROLL_PERIOD



#define NUMBELLS 3
#define B 1 << 0
#define A 1 << 1
#define G 1 << 2
#define R 0

#define P2_1 B
#define P1_5 A
#define P9_4 G



extern int noteIndex = -1;
extern int rollIndex = -1;
extern int changeFlag = 0;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                   // Stop Watchdog timer
    PM5CTL0 &= ~LOCKLPM5;                       // Disable the GPIO power-on default high-impedance mode

    __enable_interrupt();


    P2DIR |= BIT1;                              // Set P2.1 to output direction
    P1DIR |= BIT5;                              // Set P1.5 to output direction
    P9DIR |= BIT4;                              // Set P9.4 to output direction

    TA0CCR0 = ROLL_PERIOD;
    TA0CCTL0 |= CCIE;
    TA0CTL = TASSEL_2 + ID_3 + MC_1; //Select SMCLK, SMCLK/1, Up Mode

    int rollStates[NUMBELLS];
    int bell;
    for (bell = 0; bell < NUMBELLS; bell++)
    {
        rollStates[bell] = 0;
    }



    uint32_t carolOfTheBells[CAROL_LEN] = { B, R, A, R, G, R, G, R };

    while (1)
    {

        if (changeFlag == 1)
        {
            changeFlag = 0;

        }
        else
        {
            continue;
        }

        if (rollIndex == CAROL_ROLLS_PER_NOTE)
        {
            rollIndex = 0;
            noteIndex++;
            if (noteIndex == CAROL_LEN)
            {
                noteIndex = 0;
            }
            for (bell = 0; bell < NUMBELLS; bell++)
            {
                rollStates[bell] = 0;
            }
        }
        else
        {
            if (rollStates[0])
            {
                P2OUT |= BIT1;
            }
            if (rollStates[1])
            {
                P1OUT |= BIT5;
            }
            if (rollStates[2])
            {
                P9OUT |= BIT4;
            }
            __delay_cycles(SIG_LEN);
            P2OUT &= ~BIT1;
            P1OUT &= ~BIT5;
            P9OUT &= ~BIT4;
            continue;
        }


        if ((carolOfTheBells[noteIndex] & P2_1) == P2_1)
        {
            P2OUT |= BIT1;
        }
        if ((carolOfTheBells[noteIndex] & P1_5) == P1_5)
        {
            P1OUT |= BIT5;
            rollStates[1] = 1;
        }
        if ((carolOfTheBells[noteIndex] & P9_4) == P9_4)
        {
            P9OUT |= BIT4;
        }
        __delay_cycles(SIG_LEN);
        P2OUT &= ~BIT1;
        P1OUT &= ~BIT5;
        P9OUT &= ~BIT4;


    }
}


#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
{
    rollIndex++;
    changeFlag = 1;
}
