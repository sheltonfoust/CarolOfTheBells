#include <msp430.h> 
#include <stdint.h>

#define SIG_LEN 7500
#define MS 1000 / 8

#define CAROL_LEN 41 * 6
#define START_LEN 38




#define CAROL_TEMPO 120
#define START_TEMPO 480




#define NUMBELLS 12

#define E_6 11
#define D_6 10
#define C_6 9
#define B_5 8 
#define A_5 7
#define GS5 6
#define G_5 5
#define FS5 4
#define E_5 3
#define DS5 2
#define CS4 1
#define B_4 0

#define HE      1 << E_6
#define HD      1 << D_6
#define C       1 << C_6
#define B       1 << B_5
#define A       1 << A_5
#define GS      1 << GS5
#define G       1 << G_5
#define FS      1 << FS5
#define E       1 << E_5
#define DS      1 << DS5
#define CS      1 << CS4
#define LB      1 << B_4

#define R       0
#define R2      0,0
#define R3      0,0,0
#define R4      0,0,0,0
#define R6      0,0,0,0,0,0
#define R8      0,0,0,0,0,0,0,0


#define P3_7 HE
#define P3_6 HD
#define P3_3 C
#define P2_6 B
#define P2_7 A
#define P4_7 GS
#define P2_4 G
#define P2_5 FS
#define P1_7 E
#define P1_6 DS
#define P1_5 CS
#define P2_1 LB


void playNote(uint32_t* song, int MSperiod);


extern int noteIndex = 0;
extern int32_t tickIndex = -1;
extern int changeFlag = 0;




uint32_t startUpSong[START_LEN] =
{
     R4,
     LB, CS, DS, E, FS, G, GS, A, B, C, HD, HE,
     R6,
     HE, HD, C, B, A, GS, G, FS, E, DS, CS, LB,
     R4
};



uint32_t carolOfTheBells[CAROL_LEN] =
{
    R6,
    G,  R,      FS, G,      E,  R, // 12 measures of main theme
    G,  R,      FS, G,      E,  R,
    G,  R,      FS, G,      E,  R,
    G,  R,      FS, G,      E,  R,

    G,  R,      FS, G,      E,  R,
    G,  R,      FS, G,      E,  R,
    G,  R,      FS, G,      E,  R,
    G,  R,      FS, G,      E,  R,

    G,  R,      FS, G,      E,  R,
    G,  R,      FS, G,      E,  R,
    G,  R,      FS, G,      E,  R,
    G,  R,      FS, G,      E,  R,


    B,  R,      A,  B,      G,  R,
    B,  R,      A,  B,      G,  R,
    B,  R,      A,  B,      G,  R,
    B,  R,      A,  B,      G,  R,

    HE,  R,     HE, HE,     HD, C,
    B,  R,      B,  B,      A,  G,
    A,  R,      A,  A,      B,  A,
    G,  R,      FS, G,      E,  R,

    LB, CS,     DS, E,      FS, G,
    A,  B,      A,  R,      G,  R,
    LB, CS,     DS, E,      FS, G,
    A,  B,      A,  R,      G,  R,


    G,  R,      FS, G,      E,  R, // back to main theme (but only for 8 measures)
    G,  R,      FS, G,      E,  R,
    G,  R,      FS, G,      E,  R,
    G,  R,      FS, G,      E,  R,

    G,  R,      FS, G,      E,  R,
    G,  R,      FS, G,      E,  R,
    G,  R,      FS, G,      E,  R,
    G,  R,      R, FS,      R,  G,
    R3,  E,      R2,
    R6,

    R2, B,  R3,
    A, R2,  B,     R2,
    R3, E,  R2,
    R6,
    R6,
    R6,
};






int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                   // Stop Watchdog timer
    PM5CTL0 &= ~LOCKLPM5;                       // Disable the GPIO power-on default high-impedance mode

    __enable_interrupt();

    // Set output directions
    P3DIR |= BIT7;
    P3DIR |= BIT6;
    P3DIR |= BIT3;
    P2DIR |= BIT6;
    P2DIR |= BIT7;
    P4DIR |= BIT7;
    P2DIR |= BIT4;
    P2DIR |= BIT5;
    P1DIR |= BIT7;
    P1DIR |= BIT6;
    P1DIR |= BIT5;
    P2DIR |= BIT1;

    P1OUT = 0;
    P2OUT = 0;
    P3OUT = 0;
    P4OUT = 0;

    TA0CCR0 = MS;
    TA0CCTL0 |= CCIE;
    TA0CTL = TASSEL_2 + ID_3 + MC_1; //Select SMCLK, SMCLK/1, Up Mode


    while (noteIndex < START_LEN)
    {
        playNote(startUpSong, START_TEMPO);
    }


    noteIndex = 0;

    while (1)
    {

        if (noteIndex == CAROL_LEN)
        {
            noteIndex = 0;
        }

        playNote(carolOfTheBells, CAROL_TEMPO);

    }
}

void playNote(uint32_t* song, int tempo)
{
    int period = 30000 / tempo;
    if (changeFlag == 1)
    {
        changeFlag = 0;
        if (tickIndex < period)
        {
            return;
        }
        tickIndex = 0;



        if ((song[noteIndex] & P3_7) == P3_7)
        {
            P3OUT |= BIT7;
        }
        if ((song[noteIndex] & P3_6) == P3_6)
        {
            P3OUT |= BIT6;
        }
        if ((song[noteIndex] & P3_3) == P3_3)
        {
            P3OUT |= BIT3;
        }
        if ((song[noteIndex] & P2_6) == P2_6)
        {
            P2OUT |= BIT6;
        }
        if ((song[noteIndex] & P2_7) == P2_7)
        {
            P2OUT |= BIT7;
        }
        if ((song[noteIndex] & P4_7) == P4_7)
        {
            P4OUT |= BIT7;
        }
        if ((song[noteIndex] & P2_4) == P2_4)
        {
            P2OUT |= BIT4;
        }
        if ((song[noteIndex] & P2_5) == P2_5)
        {
            P2OUT |= BIT5;
        }
        if ((song[noteIndex] & P1_7) == P1_7)
        {
            P1OUT |= BIT7;
        }
        if ((song[noteIndex] & P1_6) == P1_6)
        {
            P1OUT |= BIT6;
        }
        if ((song[noteIndex] & P1_5) == P1_5)
        {
            P1OUT |= BIT5;
        }
        if ((song[noteIndex] & P2_1) == P2_1)
        {
            P2OUT |= BIT1;
        }

        __delay_cycles(SIG_LEN);

        P3OUT &= ~BIT7;
        P3OUT &= ~BIT6;
        P3OUT &= ~BIT3;
        P2OUT &= ~BIT6;
        P2OUT &= ~BIT7;
        P4OUT &= ~BIT7;
        P2OUT &= ~BIT4;
        P2OUT &= ~BIT5;
        P1OUT &= ~BIT7;
        P1OUT &= ~BIT6;
        P1OUT &= ~BIT5;
        P2OUT &= ~BIT1;


        noteIndex++;

    }
}



#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
{
    tickIndex++;
    changeFlag = 1;
}
