#include <msp430.h> 
#include <stdint.h>

#define SIG_LEN 7500
#define TRILL_LEN 7500
#define MS 1000 / 8

#define START_LEN 38
#define START_TEMPO 480

#define BING_LEN 24
#define BING_TEMPO 70

#define CAROL_LEN 41 * 6
#define CAROL_TEMPO 120

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

#define HE      (1 << E_6)
#define HD      (1 << D_6)
#define C       (1 << C_6)
#define B       (1 << B_5)
#define A       (1 << A_5)
#define GS      (1 << GS5)
#define G       (1 << G_5)
#define FS      (1 << FS5)
#define E       (1 << E_5)
#define DS      (1 << DS5)
#define CS      (1 << CS4)
#define LB      (1 << B_4)

// For some reason I can't bit shift over 16
#define HET     0b100000000000000000000000
#define HDT     0b10000000000000000000000
#define C_T     0b1000000000000000000000
#define B_T     0b100000000000000000000
#define A_T     0b10000000000000000000
#define GST     0b1000000000000000000
#define G_T     0b100000000000000000
#define FST     0b10000000000000000
#define E_T     0b1000000000000000
#define DST     0b100000000000000
#define CST     0b10000000000000
#define LBT     0b1000000000000



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


#define P3_7T HET
#define P3_6T HDT
#define P3_3T C_T
#define P2_6T B_T
#define P2_7T A_T
#define P4_7T GST
#define P2_4T G_T
#define P2_5T FST
#define P1_7T E_T
#define P1_6T DST
#define P1_5T CST
#define P2_1T LBT



typedef struct _Song
{
    uint32_t* notes;
    int length;
    int tempo;
} Song;


void playNote();


extern int noteIndex = 0;
extern int32_t tickIndex = -1;
extern int tickChangeFlag = 0;

extern int songChangeFlag = 0;
Song selectedSong;
extern uint32_t trillStates = 0;






uint32_t startUpNotes[START_LEN] =
{
     R4,
     LB, CS, DS, E, FS, G, GS, A, B, C, HD, HE,
     R6,
     HE, HD, C, B, A, GS, G, FS, E, DS, CS, LB,
     R4
};



uint32_t carolOfTheBellsNotes[CAROL_LEN] =
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

uint32_t bingBongNotes[BING_LEN] =
{
         HE,  R,     HE, HE,     HD, C,
         B,  R,      B,  B,      A,  G,
         A,  R,      A,  A,      B,  A,
         G,  R,      FS, G,      E,  R,
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

    Song carolOfTheBells;
    Song bingBong;
    Song startUp;

    carolOfTheBells.notes = carolOfTheBellsNotes;
    carolOfTheBells.length = CAROL_LEN;
    carolOfTheBells.tempo = CAROL_TEMPO;

    bingBong.notes = bingBongNotes;
    bingBong.length = BING_LEN;
    bingBong.tempo = BING_TEMPO;

    startUp.notes = startUpNotes;
    startUp.length = START_LEN;
    startUp.tempo = START_TEMPO;

    selectedSong = startUp;
    while (noteIndex < selectedSong.length)
    {
        playNote();
    }


    noteIndex = 0;

    selectedSong = carolOfTheBells;
    songChangeFlag = 0;
    while(1)
    {

        if (songChangeFlag == 1)
        {
            noteIndex = 0;
            songChangeFlag = 0;
            if (selectedSong.notes == bingBong.notes)
            {
                selectedSong = carolOfTheBells;
            }
            else if (selectedSong.notes == carolOfTheBells.notes)
            {
                selectedSong = bingBong;
            }

        }

        if (noteIndex == selectedSong.length)
        {
            noteIndex = 0;
        }

        playNote();

    }
}


void playNote()
{
    uint32_t* song = selectedSong.notes;
    int period = 30000 / selectedSong.tempo;
    int32_t bigIntPeriod = period;
    int32_t msperiod = bigIntPeriod * 1000;
    int32_t trillCounter = msperiod / SIG_LEN / 9;
    if (tickChangeFlag == 1)
    {
        tickChangeFlag = 0;
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

        __delay_cycles(4 * SIG_LEN);
        __delay_cycles(4 * SIG_LEN);

        trillCounter -= 1;
        while (trillCounter > 0)
        {
            trillCounter -= 1;

            if ((song[noteIndex] & P3_7T) == P3_7T)
            {
                P3OUT |= BIT7;

            }
            if ((song[noteIndex] & P3_6T) == P3_6T)
            {
                P3OUT |= BIT6;
            }
            if ((song[noteIndex] & P3_3T) == P3_3T)
            {
                P3OUT |= BIT3;
            }
            if ((song[noteIndex] & P2_6T) == P2_6T)
            {
                P2OUT |= BIT6;
            }
            if ((song[noteIndex] & P2_7T) == P2_7T)
            {
                P2OUT |= BIT7;
            }
            if ((song[noteIndex] & P4_7T) == P4_7T)
            {
                P4OUT |= BIT7;
            }
            if ((song[noteIndex] & P2_4T) == P2_4T)
            {
                P2OUT |= BIT4;
            }
            if ((song[noteIndex] & P2_5T) == P2_5T)
            {
                P2OUT |= BIT5;
            }
            if ((song[noteIndex] & P1_7T) == P1_7T)
            {
                P1OUT |= BIT7;
            }
            if ((song[noteIndex] & P1_6T) == P1_6T)
            {
                P1OUT |= BIT6;
            }
            if ((song[noteIndex] & P1_5T) == P1_5T)
            {
                P1OUT |= BIT5;
            }
            if ((song[noteIndex] & P2_1T) == P2_1T)
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

            __delay_cycles(4 * SIG_LEN);
            __delay_cycles(4 * SIG_LEN);
        }




        noteIndex++;


    }
}



#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
{
    tickIndex++;
    tickChangeFlag = 1;
}
