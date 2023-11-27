#include <msp430.h> 
#include <stdint.h>
#include <string.h>

#define SIG_LEN 7500
#define TRILL_LEN 7500
#define MS 1000 / 8

#define START_LEN 74
#define START_TEMPO 90

#define CAROL 0
#define BING 1
#define NUM_SONGS 2

#define BING_LEN 24
#define BING_TEMPO 70

#define CAROL_LEN 41 * 6
#define CAROL_TEMPO 120

#define NUMBELLS 12

#define MAX_LEN 50
#define SCROLL_TIME 250000

#define WELCOME_MSG "PRESS BUTTON TO START "

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
#define P8_4 C
#define P2_6 B
#define P2_7 A
#define P1_3 GS
#define P2_4 G
#define P2_5 FS
#define P1_7 E
#define P1_6 DS
#define P2_2 CS
#define P2_1 LB


#define P3_7T HET
#define P3_6T HDT
#define P8_4T C_T
#define P2_6T B_T
#define P2_7T A_T
#define P1_3T GST
#define P2_4T G_T
#define P2_5T FST
#define P1_7T E_T
#define P1_6T DST
#define P2_2T CST
#define P2_1T LBT



typedef struct _Song
{
    uint32_t* notes;
    int length;
    int tempo;
} Song;


void playNote();
void showChar(char c, unsigned short position);
void wait(int numTimePeriods);
extern void LCDini(void);


extern int playState = 0;
extern int noteIndex = 0;
extern int32_t tickIndex = -1;
extern int tickChangeFlag = 0;
extern int playFlag = 0;
extern int songChangedFlag = 0;
Song selectedSong;
extern uint32_t trillStates = 0;


// The Position accounts for Offset of Alphanumeric character positions in the LCD Memory Map
short Pos[6] = {9, 5, 3, 18, 14, 7}; // Define and populate Position, Range 0 - 5

char text[MAX_LEN];
int scrollPos;
int song;
uint32_t scrollTimeIndex;



uint32_t startUpNotes[START_LEN] =
{
     R4,
     LB, LB, LB, LB,
     CS, CS, CS, CS,
     DS, DS, DS, DS,
     E, E, E, E,
     FS, FS, FS, FS,
     G, G, G, G,
     GS, GS, GS, GS,
     A, A, A, A,
     B, B, B, B,
     C, C, C, C,
     HD, HD, HD, HD,
     HE, HE, HE, HE,
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





    LCDini();                                   // Initialize the LCD

    // ******************************************************************************************
    // **********                   CAROL OF THE BELLS                                ***********
    // **********                        LCD CODE                                     ***********
    // **********               SCROLLS ACROSS DISPLAY                                ***********
    // ******************************************************************************************

        P1DIR &= ~BIT1;                             // Set P1.1 to input direction
        P1REN |= BIT1;                              // Enable Resistor on P1.1, Button 1
        P1OUT |= BIT1;                              // Enable Pull-up  on P1.1, Button 1
        P1DIR &= ~BIT2;                             // Set P1.2 to input direction
        P1REN |= BIT2;                              // Enable Resistor on P1.1, Button 2
        P1OUT |= BIT2;                              // Enable Pull-up  on P1.1, Button 2

        //  Configure P1.1 & P1.2 for falling edge Interrupt
            P1IES |=  BIT1;                             // Select Interrupts for HIGH to LOW Transition
            P1IFG &= ~BIT1;                             // Clear  P1.1 Interrupt flag
            P1IE  |=  BIT1;                             // Enable P1.1 Interrupt
            P1IES |=  BIT2;                             // Select Interrupts for HIGH to LOW Transition
            P1IFG &= ~BIT2;                             // Clear  P1.2 Interrupt flag
            P1IE  |=  BIT2;                             // Enable P1.2 Interrupt



    // Set output directions
    P3DIR |= BIT7;
    P3DIR |= BIT6;
    P8DIR |= BIT4;
    P2DIR |= BIT6;
    P2DIR |= BIT7;
    P1DIR |= BIT3;
    P2DIR |= BIT4;
    P2DIR |= BIT5;
    P1DIR |= BIT7;
    P1DIR |= BIT6;
    P2DIR |= BIT2;
    P2DIR |= BIT1;


    TA0CCR0 = MS;
    TA0CCTL0 |= CCIE;
    TA0CTL = TASSEL_2 + ID_3 + MC_1; //Select SMCLK, SMCLK/1, Up Mode

    __enable_interrupt();


    strncpy(text, WELCOME_MSG, strlen(WELCOME_MSG));
    scrollPos = 0;
    scrollTimeIndex = 0;




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
    songChangedFlag = 0;
    while(1)
    {

        if (songChangedFlag == 1)
        {
            noteIndex = 0;
            songChangedFlag = 0;

            song++;
            if (song == NUM_SONGS)
            {
                song = 0;
            }

            if (song == BING)
            {
                selectedSong = bingBong;
            }
            else if (song == CAROL)
            {
                selectedSong = carolOfTheBells;
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
        if ((song[noteIndex] & P8_4) == P8_4)
        {
            P8OUT |= BIT4;
        }
        if ((song[noteIndex] & P2_6) == P2_6)
        {
            P2OUT |= BIT6;
        }
        if ((song[noteIndex] & P2_7) == P2_7)
        {
            P2OUT |= BIT7;
        }
        if ((song[noteIndex] & P1_3) == P1_3)
        {
            P1OUT |= BIT3;
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
        if ((song[noteIndex] & P2_2) == P2_2)
        {
            P2OUT |= BIT2;
        }
        if ((song[noteIndex] & P2_1) == P2_1)
        {
            P2OUT |= BIT1;
        }




        wait(1);

        P3OUT &= ~BIT7;
        P3OUT &= ~BIT6;
        P8OUT &= ~BIT4;
        P2OUT &= ~BIT6;
        P2OUT &= ~BIT7;
        P1OUT &= ~BIT3;
        P2OUT &= ~BIT4;
        P2OUT &= ~BIT5;
        P1OUT &= ~BIT7;
        P1OUT &= ~BIT6;
        P2OUT &= ~BIT2;
        P2OUT &= ~BIT1;

        wait(4);
        wait(4);

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
            if ((song[noteIndex] & P8_4T) == P8_4T)
            {
                P8OUT |= BIT4;
            }
            if ((song[noteIndex] & P2_6T) == P2_6T)
            {
                P2OUT |= BIT6;
            }
            if ((song[noteIndex] & P2_7T) == P2_7T)
            {
                P2OUT |= BIT7;
            }
            if ((song[noteIndex] & P1_3T) == P1_3T)
            {
                P1OUT |= BIT3;
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
            if ((song[noteIndex] & P2_2T) == P2_2T)
            {
                P2OUT |= BIT2;
            }
            if ((song[noteIndex] & P2_1T) == P2_1T)
            {
                P2OUT |= BIT1;
            }
            wait(1);

            P3OUT &= ~BIT7;
            P3OUT &= ~BIT6;
            P8OUT &= ~BIT4;
            P2OUT &= ~BIT6;
            P2OUT &= ~BIT7;
            P1OUT &= ~BIT3;
            P2OUT &= ~BIT4;
            P2OUT &= ~BIT5;
            P1OUT &= ~BIT7;
            P1OUT &= ~BIT6;
            P2OUT &= ~BIT2;
            P2OUT &= ~BIT1;

            wait(4);
            wait(4);
        }




        noteIndex++;


    }
}


void wait(int numTimePeriods)
{
    int timePeriodIndex = 0;

    while(timePeriodIndex < numTimePeriods)
    {
        if (scrollTimeIndex >= SCROLL_TIME)
        {
            scrollTimeIndex = 0;
            int textLen = strlen(text);
            int i;
            for (i = 0; i < 6; i++)
            {
                showChar(text[(scrollPos + i) % textLen], Pos[i]);
            }

            scrollPos = (scrollPos + 1) % textLen;
        }
        else
        {
            scrollTimeIndex += SIG_LEN;
        }
        __delay_cycles(SIG_LEN);
        timePeriodIndex++;
    }
}




void showChar(char c, unsigned short position)
{
// Space/Number Characters:  [  SP   ]  [   !   ]  [   "   ]  [   #   ]  [   $   ]  [   %   ]  [   &   ]  [   '   ]  [   (   ]  [   )   ]  [   *   ]  [   +   ]  [   ,   ]
const char sp_num[26][2]  = {0x00,0x00, 0x60,0x01, 0x40,0x40, 0x61,0x50, 0xB7,0x50, 0x00,0x29, 0x10,0xCA, 0x00,0x20, 0x00,0x22, 0x00,0x88, 0x03,0xAA, 0x03,0x50, 0x00,0x08,
                             0x03,0x00, 0x00,0x01, 0x00,0x28, 0xFC,0x28, 0x60,0x20, 0xDB,0x00, 0xF3,0x00, 0x67,0x00, 0xB7,0x00, 0xBF,0x00, 0xE4,0x00, 0xFF,0x00, 0xF7,0x00};
// Space/Number Characters:  [   -   ]  [   .   ]  [   /   ]  [   0   ]  [   1   ]  [   2   ]  [   3   ]  [   4   ]  [   5   ]  [   6   ]  [   7   ]  [   8   ]  [   9   ]

// Upper Case Characters:    [   A   ]  [   B   ]  [   C   ]  [   D   ]  [   E   ]  [   F   ]  [   G   ]  [   H   ]  [   I   ]  [   J   ]  [   K   ]  [   L   ]  [   M   ]
const char  upper[26][2]  = {0xEF,0x00, 0xF1,0x50, 0x9C,0x00, 0xF0,0x50, 0x9F,0x00, 0x8F,0x00, 0xBD,0x00, 0x6F,0x00, 0x90,0x50, 0x78,0x00, 0x0E,0x22, 0x1C,0x00, 0x6C,0xA0,
                             0x6C,0x82, 0xFC,0x00, 0xCF,0x00, 0xFC,0x02, 0xCF,0x02, 0xB7,0x00, 0x80,0x50, 0x7C,0x00, 0x0C,0x28, 0x6C,0x0A, 0x00,0xAA, 0x00,0xB0, 0x90,0x28};
// Upper Case Characters:    [   N   ]  [   O   ]  [   P   ]  [   Q   ]  [   R   ]  [   S   ]  [   T   ]  [   U   ]  [   V   ]  [   W   ]  [   X   ]  [   Y   ]  [   Z   ]

// Lower Case Characters:    [   a   ]  [   b   ]  [   c   ]  [   d   ]  [   e   ]  [   f   ]  [   g   ]  [   h   ]  [   i   ]  [   J   ]  [   k   ]  [   l   ]  [   m   ]
const char  lower[26][2]  = {0x21,0x12, 0x3F,0x00, 0x1B,0x00, 0x7B,0x00, 0x1A,0x08, 0x0E,0x00, 0xF7,0x00, 0x2F,0x00, 0x10,0x10, 0x78,0x00, 0x00,0x72, 0x0C,0x00, 0x2B,0x10,
                             0x21,0x10, 0x0A,0x18, 0xCF,0x00, 0xE7,0x00, 0x0A,0x00, 0x11,0x02, 0x03,0x10, 0x38,0x00, 0x08,0x08, 0x28,0x0A, 0x00,0xAA, 0x00,0xA8, 0x12,0x08};
// Lower Case Characters:    [   n   ]  [   o   ]  [      ]  [   q   ]  [   r   ]  [   s   ]  [   t   ]  [   u   ]  [   v   ]  [   w   ]  [   x   ]  [   y   ]  [   z   ]

    if (c >= ' ' && c <= '9')
      {
        // Display special printable characters & Numbers
        LCDMEM[position]   =  sp_num[c-32][0];
        LCDMEM[position+1] =  sp_num[c-32][1];
      }
    else if (c >= 'A' && c <= 'Z')
      {
        // Display Upper-case Letters
        LCDMEM[position]   = upper[c-65][0];
        LCDMEM[position+1] = upper[c-65][1];
      }
    else if (c >= 'a' && c <= 'z')
      {
        // Display lower-case letters
        LCDMEM[position]   = lower[c-97][0];
        LCDMEM[position+1] = lower[c-97][1];
      }
    else
      {
        // Turn all segments on for other character
        LCDMEM[position]   = 0xFF;
        LCDMEM[position+1] = 0xFA;
      }
}


#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
{
    tickIndex++;
    tickChangeFlag = 1;
}


#pragma vector = PORT1_VECTOR
__interrupt void PORT1_ISR(void)
{
    __delay_cycles(200000);
 // De-bounce using MSP Library Routine, 200ms @ 1MHz

    if (P1IFG & BIT1) // If Button 1 is pressed
    {
        // When BTN1 is pressed
        playFlag = 1;
        P1IFG &= ~BIT1; // Clear P1.1 Interrupt flag
    }

    if (P1IFG & BIT2) // If Button 2 is pressed
    {
        // When BTN2 is pressed
        songChangedFlag = 1;
        P1IFG &= ~BIT2; // Clear P1.2 Interrupt flag
    }
}

void LCDini(void)
{
// ******************************************************************************************
// *****  Basic LCD Initialization Copyright (c) 2014, Texas Instruments Incorporated   *****
// ******************************************************************************************
//  This examples configures the LCD in 4-Mux mode.
//  The internal voltage is sourced to V2 through V4 and V5
//  is connected to ground. Charge pump is enabled.
//  It uses LCD pin L0~L21 and L26~L43 as segment pins.
//  f(LCD) = 32768Hz/((1+1)*2^4) = 1024Hz, ACLK = 32768Hz,
//  MCLK = SMCLK = default DCODIV 1MHz.
//
//      MSP430FR6989 / MSP-EXP430FR6989 Launchpad
//              -----------------
//          /|\|                 |
//           | |              XIN|--
//  GND      --|RST              |  32768Hz
//   |         |             XOUT|--
//   |         |                 |
//   |         |             COM3|----------------|
//   |         |             COM2|---------------||
//   |--4.7uF -|LCDCAP       COM1|--------------|||
//             |             COM0|-------------||||
//             |                 |    -------------
//             |           Sx~Sxx|---| 1 2 3 4 5 6 |
//             |                 |    -------------
//             |                 |       TI LCD
//                                 (See MSP-EXP430FR6989 Schematic)
//
//*****************************************************************************
// Initialize LCD segments 0 - 21; 26 - 43
   LCDCPCTL0 = 0xFFFF;
   LCDCPCTL1 = 0xFC3F;
   LCDCPCTL2 = 0x00FF;

    PJSEL0 = BIT4 | BIT5;                      // Turn on LFXT (Low Frequency Crystal Oscillator)
// Configure LFXT 32kHz Crystal Oscillator
   CSCTL0_H = CSKEY >> 8;                      // Unlock CS registers
   CSCTL4 &= ~LFXTOFF;                         // Enable LFXT  (Low Frequency Crystal Oscillator)
         do
           {
            CSCTL5 &= ~LFXTOFFG;               // Clear LFXT fault flag
            SFRIFG1 &= ~OFIFG;
            }
            while (SFRIFG1 & OFIFG);           // Test oscillator fault flag
            CSCTL0_H = 0;                      // Lock CS registers

// Initialize LCD_Clock
// ACLK, Divider = 1, Pre-divider = 16; 4-pin MUX
            LCDCCTL0 = LCDDIV__1 | LCDPRE__16 | LCD4MUX | LCDLP;

// VLCD generated internally,
// V2-V4 generated internally, V5 to ground
// Set VLCD voltage to 2.60V
// Enable charge pump and select internal reference for it
           LCDCVCTL     =   VLCD_1 | VLCDREF_0 | LCDCPEN;
           LCDCCPCTL    = LCDCPCLKSYNC;         // Clock synchronization enabled
           LCDCMEMCTL   = LCDCLRM;              // Clear LCD memory
// Turn LCD on
           LCDCCTL0 |= LCDON;
}
