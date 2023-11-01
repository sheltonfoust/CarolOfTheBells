#include "msp430.h"
extern void LCDini(void);                               // Declare LCDini Routine
extern void showBatt(unsigned short bar);               // Declare the Show Battery Routine
extern void showINT(int Val);      // Declare Show Integer Value Routine
extern void showChar(char c, unsigned short position);  // Declare Show Character Routine
// The Position accounts for Offset of Alphanumeric character positions in the LCD Memory Map
short  Pos[6]  = { 9, 5, 3, 18, 14, 7 };                // Define and populate Position, Range 0 - 5

//################################################
//#                                             ##
//#     Travis Dickens R#11756425 voltometer    ##
//#                                             ##
//################################################

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                           // Stop WDT
    PM5CTL0 &= ~LOCKLPM5;                               // Disable the GPIO power-on default high-impedance mode

    LCDini();                                           // Initialize the LCD
    unsigned short lvl = 0;                             // Define and initialize lvl
    showBatt(lvl);                                      // Display Battery level
    showChar('D', Pos[0]);                              // Show 'D' for Decimal

//  GPIO Setup
    P1OUT &= ~BIT0;                                     // Clear Red   LED
    P9OUT &= ~BIT7;                                     // Clear Green LED
    P1DIR  |= BIT0;                                     // Set P1.0 / Red   LED to output
    P9DIR  |= BIT7;                                     // Set P9.7 / Green LED to output
    P8SEL1 |= BIT5;                                     // Configure P8.5 for ADC(6)
    P8SEL0 |= BIT5;                                     // Configure P8.5 for ADC(6)
    P8SEL1 |= BIT4;                                     // Configure P8.4 for ADC(7)
    P8SEL0 |= BIT4;                                     // Configure P8.4 for ADC(7)

 // ******************************************************************************************
 // *******************     Initialize the shared Reference module         *******************
 // ******************************************************************************************
    while(REFCTL0 & REFGENBUSY);                        // WAIT if Reference Generator busy
 // REFCTL0 |= BIT4;                                    // Select 2.0V / Default is 1.2V
    REFCTL0 |= BIT5;                                    // Select 2.5V / Default is 1.2V
    REFCTL0 |= REFON;                                   // Enable shared Reference Generator

 // ******************************************************************************************
 // **********   Configure AD-Converter for Analog Input on Ch. 7, March 20, 2023  ***********
 // ******************************************************************************************
    ADC12CTL0 &= ~ADC12ENC;                             // Disable ADC12
    ADC12CTL0   = ADC12SHT0_3 | ADC12ON;                // Sampling time, S&H=32, ADC12 on
    ADC12CTL1   = ADC12SHP;                             // ADCCLK = MODOSC; sampling timer
 // *******************         Select 12 / 10 /08 Bits Resolution          *******************
 // ADC12CTL2  &= ~BIT5;                                // Turn 12 Bit Mode off
 // ADC12CTL2  |= BIT4;                                 // 10 Bit Mode on
 // *******************   Select Inputs Analog 6 / 7 / Temp / 0.5Vcc        *******************
 // ADC12CTL3  = ADC12TCMAP;                            // Enable internal temperature sensor
 // ADC12MCTL0 = ADC12INCH_30;                          // Vref = AVcc, ADC input ch A30 => temp sense
 // ADC12CTL3  = ADC12BATMAP;                           // Enable 1/2 Vcc
 // ADC12MCTL0 = ADC12INCH_31;                          // ADC input ch A31 => 1/2 Vcc
 // ADC12MCTL0 |= ADC12INCH_6;                          // A6 ADC input select; (Vref = AVcc default)
    ADC12MCTL0 |= ADC12INCH_7;                          // A7 ADC input select; (Vref = AVcc default)

 // ********************        Select the shared Reference Module       *********************
 // ADC12MCTL0 |= ADC12VRSEL_1;                         // Vref = selected Reference Voltage
 // while(REFCTL0 & REFGENBUSY);                        // WAIT if Reference Generator busy

    ADC12IER0  |= ADC12IE0;                             // Enable ADC conversion complete interrupt
    ADC12CTL0  |= ADC12ENC;                             // Enable ADC12
    __enable_interrupt();                               // Enable interrupts


    while (1)
        {
        __delay_cycles(100000);                         // Delay 100 ms
        ADC12CTL0 |= ADC12ENC | ADC12SC;                // Start sampling/conversion
        }
}

// ******************************************************************************************
// **********     AD-Converter Conversion complete Interrupt Service Routine      ***********
// ******************************************************************************************
#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
{
            int level = 0;                              // Define and initialize Battery Bar Level
            level = 6 * ADC12MEM0 / 0xFFF;              // Scale Battery Bar Level
            showBatt(level);                            // Show Battery  Bar Level

            int value = ADC12MEM0;                      // Define and initialize AD Conversion value
            showINT(value);                         // Show value in Decimal
}

// ******************************************************************************************
// **                   Show Battery Level with 0 - 6 Bars and Battery Outline             **
// ******************************************************************************************
void showBatt(unsigned short bar)
{
// Battery Symbol Definition:   [   0   ]  [   1   ]  [   2   ]  [   3   ]  [   4   ]  [   5   ]  [   6   ]
const char batt_lvl[7][2]    = {0x10,0x10, 0x10,0x30, 0x30,0x30, 0x30,0x70, 0x70,0x70, 0x70,0xF0, 0xF0,0xF0};
// Display Battery
        LCDMEM[13]   = batt_lvl[bar][0];
        LCDMEM[17]   = batt_lvl[bar][1];
}

// ******************************************************************************************
// **********       Display Numerical Value of 16 Bit Number in selected Base     ***********
// ******************************************************************************************
void showINT(int Val)
{
    float     res     = 0;                                // Define and initialize residual value
    int     thnds    = 0;                                // Define and initialize the thousands place
    int     hunds    = 0;                                // Define and initialize the hundreds place
    int     tens    = 0;                                // Define and initialize the tens place
    int     ones    = 0;                                // Define and initialize the ones place
    double Voltage = 0;

    Voltage = (Val/4095.0)*3.3;

    ones  = Voltage;                       //ones value
    res   = 10*(Voltage-ones);          // Update Residual
    tens  = res;                            //tens value
    res   = 10*(res-tens);          // Update Residual
    hunds  = res;                                 //hundreds value
    res   = 10*(res-hunds);        // Update Residual
    thnds  = res;                       //thousands value

// ******************************************************************************************
// **********       Display Numerical Value of 16 Bit Number in selected Base     ***********
// ******************************************************************************************
        showChar('V', Pos[0]);                      // Show 'V' for Voltage


        showChar(thnds +48, Pos[5]);    //Shows thousands place
        showChar(hunds +48, Pos[4]);    //Shows hundreds place
        showChar(tens +48, Pos[3]);     //Shows tens place
        showChar(ones +48, Pos[2]);     //Shows ones place
        LCDMEM[4]   = 0x01;             //Adds decimal point
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
// Lower Case Characters:    [   n   ]  [   o   ]  [   p   ]  [   q   ]  [   r   ]  [   s   ]  [   t   ]  [   u   ]  [   v   ]  [   w   ]  [   x   ]  [   y   ]  [   z   ]

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
// Configure Low Frequency 32kHz Crystal Oscillator (LFXT)
   PJSEL0 = BIT4 | BIT5;                        // Turn on LFXT (Low Frequency Crystal Oscillator)
   CSCTL0_H = CSKEY >> 8;                       // Unlock CS registers
   CSCTL4 &= ~LFXTOFF;                          // Enable LFXT  (Low Frequency Crystal Oscillator)
   do
       {
            CSCTL5 &= ~LFXTOFFG;                // Clear LFXT fault flag
            SFRIFG1 &= ~OFIFG;                  // Clear Oscillator Fault Flag
       }
            while (SFRIFG1 & OFIFG);            // Test oscillator fault flag
            CSCTL0_H = 0;                       // Lock CS registers

// Initialize LCD_Clock
// ACLK, Divider = 1, Pre-divider = 16; 4-pin MUX
   LCDCCTL0 = LCDDIV__1 | LCDPRE__16 | LCD4MUX | LCDLP;

// VLCD generated internally,
// V2-V4 generated internally, V5 to ground
// Set VLCD voltage to 2.60V
// Enable charge pump and select internal reference for it
   LCDCVCTL     = VLCD_1 | VLCDREF_0 | LCDCPEN; // Configure LCD Voltages
   LCDCCPCTL    = LCDCPCLKSYNC;                 // Clock synchronization enabled
   LCDCMEMCTL   = LCDCLRM;                      // Clear LCD memory
   LCDCCTL0     |= LCDON;                       // Turn LCD on
}
