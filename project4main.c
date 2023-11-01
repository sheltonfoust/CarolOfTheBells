#include <msp430.h>
extern int count = 0;                           // Define and initialize count
extern void showChar(char c, unsigned short position);
extern void showCount(unsigned int cnt);        // Define showCount Routine
extern void LCDini(void);
// The Position accounts for Offset of Alphanumeric character positions in the LCD Memory Map
short  Pos[6]  = { 9, 5, 3, 18, 14, 7 };        // Define and populate Position, Range 0 - 5

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                   // Stop Watchdog timer
    PM5CTL0 &= ~LOCKLPM5;                       // Disable the GPIO power-on default high-impedance mode

    LCDini();                                   // Initialize the LCD

// ******************************************************************************************
// **********                   Travis Dickens R#11756425                         ***********
// **********         Count Button Presses until 63, march 4, 2023                ***********
// **********               displays count as 6 digit binary                      ***********
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

    showCount(count);                           // Show Count

    __bis_SR_register(LPM3_bits | GIE);         // Enter low power mode with Interrupts enabled
    __no_operation();
}

// ******************************************************************************************
// *****        Interrupt Routine that detects Button 1 and Button 2 Key presses        *****
// ******************************************************************************************
#pragma vector = PORT1_VECTOR
__interrupt void PORT1_ISR(void)
{
            __delay_cycles(200000);             // De-bounce using MSP Library Routine, 200ms @ 1MHz

            if ( (P1IFG & BIT1) && count < 63 ) // If Button 1 is pressed and count < 99
                  {
                    count = count + 1;          // Increment count
                    P1IFG &= ~BIT1;             // Clear P1.1 Interrupt flag
                  }

            if ( (P1IFG & BIT2) && count > 0 )  // If Button 2 is pressed and count > 0
                  {
                    count = count - 1;          // Decrement count
                    P1IFG &= ~BIT2;             // Clear P1.2 Interrupt flag
                   }

            showCount(count);                   // Show Count
}

// ******************************************************************************************
// *****                            Display Count on LCD Screen                          *****
// ******************************************************************************************
void showCount(unsigned int cnt)
{
    int arr[6] = {0};
       int num = cnt;
       int i = 5;
       int b = 0;
       while(i >= 0)
               {
                   arr[i] = num & 1;
                   i--;
                   num >>= 1;
               }
       while(b < 6){
           showChar(arr[b] +48, Pos[b] );                 // Show Ten at Position 5
           b++;
       }
}

// *************************************************************************************************
// **  Show Character at LCD Position, works with Space, Numbers, Upper and Lower case Characters **
// *************************************************************************************************
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

/* --COPYRIGHT--
 * Copyright (c) 2014, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
