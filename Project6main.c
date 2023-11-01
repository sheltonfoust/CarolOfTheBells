#include <msp430.h>
#include <stdbool.h>
extern void LCDini(void);                       // Initialize the LCD Display
extern void showBatt(unsigned short bar);       // Define showBatt function
extern void showChar(char c, unsigned short position); // Define showChar function
extern void getTimeDate(void);                  // Get Time & Date from the Buffer
extern void showTime(void);                     // Define showTime function
extern void showDate(void);                     // Define showDate function
extern void scrollDisp(void);                   // Define function to scroll the Buffer across the LCD
extern void CheckSum(void);                     // Define CheckSum processing routine
// The Position accounts for Offset of Alphanumeric character positions in the LCD Memory Map
short  Pos[6]  = { 9, 5, 3, 18, 14, 7 };        // Define and populate Position, Range 0 - 5
extern char   GPS[300]      = {32};             // Define & Initialize GPS Data Buffer
extern unsigned int Bufsize = 300;              // Define & Initialize GPS Data Buffer Size
extern unsigned int point   = 0;                // Define & Initialize Pointer to Buffer Position
extern short  BFull         = 0;                // Define & Initialize 'Buffer full' Flag
extern short  GPSstart      = 0;                // Define & Initialize GPS Sentence Start Position
extern short  GPSstop       = 0;                // Define & Initialize GPS Sentence Stop  Position
extern short  MessCmp       = 0;                // Define & Initialize 'Message Complete' Flag
extern short  FdStart       = 0;                // Define & Initialize 'Found Start' Flag
extern char   Valid         = 'V';              // Define & Initialize 'Data Valid' Flag
extern short  Hour          = 0;                // Define & initialize Hour
extern short  Min           = 0;                // Define & initialize Minute
extern short  Sec           = 0;                // Define & initialize Second
extern short  Day           = 0;                // Define & initialize Day
extern short  Month         = 0;                // Define & initialize Month
extern short  Year          = 0;                // Define & initialize Year
extern int select           = 0;                // Define & initialize select
extern int z                = 0;                // Define & initialize z

// **************************************************************************
// ** Travis Dickens R#11756425 Project 6                                  **
// ** Toggle between reading and displaying a UTC clock and VTG speed data **
// **************************************************************************

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                   // Stop the Watch-dog timer
    PM5CTL0 &= ~LOCKLPM5;                       // Disable the GPIO power-on default high-impedance mode

    LCDini();                                   // Initialize the LCD Display / 32kHz Clock

//  Configure LEDs
    P1DIR |= BIT0;                              // Set P1.0 to output direction, Red   LED
    P9DIR |= BIT7;                              // Set P9.7 to output direction, Green LED
    P1OUT &= ~BIT0;                             // Turn Red   LED Off
    P9OUT &= ~BIT7;                             // Turn Green LED Off

// Configure PPS (P1.3) rising edge Interrupt for Data capture Synch to PPS
    P1DIR &= ~BIT3;                             // Set P1.3 to input direction, PPS Signal
    P1IES &= ~BIT3;                             // Select Interrupts for LOW to HIGH Transition
    P1IFG &= ~BIT3;                             // Clear  P1.3 Interrupt flag
    P1IE  |=  BIT3;                             // Enable P1.3 Interrupt

//  Configure AD-Converter; Input 7 / 12 Bits Res. for Battery Voltage
    P8SEL1 |= BIT4;                             // Configure P8.4 for ADC(7)
    P8SEL0 |= BIT4;                             // Configure P8.4 for ADC(7)
    ADC12CTL0 &= ~ADC12ENC;                     // Disable ADC12
    ADC12CTL0   = ADC12SHT0_3 | ADC12ON;        // Sampling time, S&H=32, ADC12 on
    ADC12CTL1   = ADC12SHP;                     // ADCCLK = MODOSC; sampling timer
    P8SEL1 |= BIT4;                             // Configure P8.4 for ADC(7)
    P8SEL0 |= BIT4;                             // Configure P8.4 for ADC(7)
    ADC12MCTL0 |= ADC12INCH_7;                  // A7 ADC input select; (Vref = AVcc default)
    ADC12IER0  |= ADC12IE0;                     // Enable ADC conversion complete interrupt
    ADC12CTL0  |= ADC12ENC;                     // Enable ADC12

// Configure GPIO for UART
    P4SEL0 |=   BIT2 | BIT3;                    // USCI_A0 UART operation, TX = MCU_OUT = P4.2
    P4SEL1 &= ~(BIT2 | BIT3);                   // USCI_A0 UART operation, RX = MCU_IN  = P4.3

//  Configure USCI_A0 for UART mode / Set Baud Rate to 9600 using High Frequency Baud Rate Generation
    UCA0CTLW0   = UCSWRST;                      // Put eUSCI (enhanced Serial Comm Interface) in Reset
    UCA0CTL1   |= UCSSEL__SMCLK;                // CLK = SMCLK  = 1.0 MHz System Clock
    UCA0BR0     = 104;                          // 1000000/9600 = 104.1666
    UCA0BR1     = 0;                            // Baud Rate Setting for SMCLK
    UCA0MCTLW   = 0x1100;                       // 0x1100 / UCBRSx = 0x11 from lookup table for 0.1666
    UCA0CTL1   &= ~UCSWRST;                     // Release from Reset
    UCA0IE     |= UCRXIE;                       // Enable USCI_A0 RX interrupt
    __enable_interrupt();                       // Enable interrupts

    ADC12CTL0 |= ADC12ENC;                      // Enable AD-Converter
    ADC12CTL0 |= ADC12SC;                       // Start AD conversion / Show Battery SOC

// ******************************************************************************************
// *****                                Main Loop                                       *****
// ******************************************************************************************
    while(1)                                    // Main Loop
    {
    unsigned int j  = 0;                        // Define & Initialize Loop counter

    if( BFull == 1 )                            // If GPS Buffer is full
       {
//  Find Start of GPS Message / Start is the position of the '$'
        GPSstart  = 0;                          // Reset pointer to GPS Message Start
        FdStart = 0;                            // Reset 'Found Start' Flag
        for( j = 0; j < Bufsize-1; j++ )        // Loop through the Data Buffer and find Start of GPS message
            {
                if(select == 0){                // Toggle
                if( (GPS[j] == '$') && (GPS[j+3] == 'R') && (GPS[j+4] == 'M') && (GPS[j+5] == 'C') && (j < (Bufsize-70)) )  // For clock data
                 {
                     FdStart  = 1;                 // Set 'Found GPS Message Start' Flag
                      GPSstart = j;                 // Set GPS Massage Start Position
                  }
                }
                else{
                if( (GPS[j] == '$') && (GPS[j+3] == 'G') && (GPS[j+4] == 'G') && (GPS[j+5] == 'A') && (j < (Bufsize-70)) )  // For VTG data
                 {
                     FdStart  = 1;                 // Set 'Found GPS Message Start' Flag
                      GPSstart = j;                 // Set GPS Massage Start Position
                  }
                }
            }
//  Find End of GPS Message / End is the position of the '*'
        GPSstop   = GPSstart;                   // Preset pointer to GPS Message Stop
        for( j = GPSstart; j < Bufsize-1; j++ ) // Loop through the Data Buffer and find Stop  of GPS message
            {
               GPSstop = j;
               if(GPS[j] == '*') break;         // Break if GPS Message Stop  is found
               if(FdStart == 0 ) break;         // Break if GPS Message Start is not found
            }
// ******************************************************************************************
// *****            Scrolling GPS Message or parts of it across LCD Screen              *****
// ******************************************************************************************
        if( FdStart == 1 )
        {
            if(select == 0){
                getTimeDate();                        // Get the Time & the Date from the GPS Message
                showDate();                           // Show UTC Time
            }
            else{
                scrollDisp();                         // Scroll GPS Message on the LCD Display
            }

            if(z < 250){                              // Alloted time past?
                select = 0;                           // Read RMC
            }
            else{
                select = 1;                           // Read VTG
                z = 0;                                // Reset counter
            }
            z++;                                      // Increment counter
            FdStart = 0;                              // Flag for recollection of data
        }

        MessCmp = 1;                              // Set 'Message Complete' Flag

      }                                           // 'Buffer full' closing bracket
        ADC12CTL0 |= ADC12SC;                     // Start AD conversion / Show Battery SOC
      }                                           // 'while(1)' closing bracket
  }
// ******************************************************************************************
// **********     AD-Converter Conversion complete Interrupt Service Routine      ***********
// ******************************************************************************************
#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
{
        int level = 0;                          // Define and initialize Battery Bar Level
        level = 6 * ADC12MEM0 / 0xF00;          // Scale Battery Bar Level
        showBatt(level);                        // Show Battery  Bar Level
}

// ******************************************************************************************
// *****                Interrupt Service Routine for rising Edge of PPS                *****
// ******************************************************************************************
#pragma vector = PORT1_VECTOR
__interrupt void PORT1_ISR(void)
{
// *****    Get new Data
        if( (MessCmp == 1) )                    // If 'Message Complete' Flag is set
          {
              BFull     = 0;                    // Reset 'Buffer Full'; Get new Data
              point     = 0;                    // Reset pointer to start of Buffer
              MessCmp   = 0;                    // Reset 'CheckSum Ok' Flag
          }
        P1IFG &= ~BIT3;                         // Clear  P1.3 Interrupt flag
}

// ******************************************************************************************
// *****  UART Interrupt Routine receiving GPS Data @ 9,600 Baud with MSP @ 1MHz Clock  *****
// *****                 RX = MCU_IN = P4.3, TX = MCU_OUT = P4.2                        *****
// ******************************************************************************************
#pragma vector = USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
    volatile char RXData = 0;                   // Define RXData variable
    RXData = UCA0RXBUF;                         // Read Receive buffer
     if( BFull == 0 )                           // If GPS Data Buffer is NOT yet full
        {
             P1OUT |= BIT0;                     // Turn Red LED On
             GPS[point] = RXData;               // Put Data in Buffer
             point = point + 1;                 // Increment Pointer
        }

     if( point == Bufsize )                     // If Buffer is filled up
        {
              BFull = 1;                        // Set "Buffer full" Flag
              P1OUT &= ~BIT0;                   // Turn Red LED Off
        }

        UCA0IFG &= ~UCRXIFG;                    // Clear interrupt Flag
}

// ******************************************************************************************
// **               Function to scroll the Buffer Content across the LCD Display           **
// ******************************************************************************************
void scrollDisp(void)
{
    int     j       = 0;                        // Define & Initialize outer loop counter
    int     i       = 0;                        // Define & Initialize inner loop counter
    char        display[6]  = "      ";         // Define and Initialize Display buffer
    for( j = GPSstart; j < GPSstop; j++ )       // Loop through the Data Buffer and scroll the contents on the LCD
         {
            for( i = 0; i < 6; i++ )            // Shift all characters in the Display to the left
                 {
                   display[i] = GPS[i+j];       // Shift message to the left
                   showChar(display[i], Pos[i]);// Show Characters on LCD
                 }
                __delay_cycles(500000);         // Wait 500 ms
         }
}

// ******************************************************************************************
// *****            Calculate and Display the Checksum of the GPS Message               *****
// ******************************************************************************************
void CheckSum(void)
{
      short Chk     = 0;                        // Declare & Initialize Checksum
      short GPSChk  = 0;                        // Declare & Initialize Checksum from GPS Sentence
      short High    = 0;                        // Define High Byte of Checksum for LCD print
      short Low     = 0;                        // Define Low  Byte of Checksum for LCD print
      int   j       = 0;                        // Define & Initialize outer loop counter
      for( j = GPSstart+1; j < GPSstop; j++ )   // Loop through the Data Buffer and calculate the checksum
          {
              Chk = Chk ^ GPS[j];               // Update  Checksum using XOR function
          }
      High =  (Chk & 0xF0) >> 4;                // Isolate High Byte Hex Character from calculated Checksum
      Low  =  (Chk & 0x0F);                     // Isolate Low  Byte Hex Character from calculated Checksum

// ******************************************************************************************
// **********       Convert 2 character ASCII Checksum in RMC Sentence to 1 Byte  ***********
// ******************************************************************************************
      if(GPS[GPSstop+1]>57) GPSChk = (GPS[GPSstop+1]-55) << 4;      // Calculate High Byte of RMC Checksum (A-F)
         else               GPSChk = (GPS[GPSstop+1]-48) << 4;      // Calculate High Byte of RMC Checksum (0-9)

      if(GPS[GPSstop+2]>57) GPSChk =  GPSChk + GPS[GPSstop+2]-55;   // Calculate Low  Byte of RMC Checksum (A-F)
          else              GPSChk =  GPSChk + GPS[GPSstop+2]-48;   // Calculate Low  Byte of RMC Checksum (0-9)
// ******************************************************************************************
// *****                Print calculated Checksum as 2 ASCII Characters                 *****
// ******************************************************************************************
      if(High > 9)  showChar(High +55, Pos[4]); // Print High Byte of calculated Checksum (A-F)
          else      showChar(High +48, Pos[4]); // Print High Byte of calculated Checksum (0-9)
      if(Low  > 9)  showChar(Low  +55, Pos[5]); // Print Low  Byte of calculated Checksum (A-F)
          else      showChar(Low  +48, Pos[5]); // Print Low  Byte of calculated Checksum (0-9)

      if( GPSChk != Chk )  LCDMEM[2] |=   BIT0; // Turn '!' Symbol on
      if( GPSChk == Chk )  LCDMEM[2] &=  ~BIT0; // Turn '!' Symbol off
      __delay_cycles(200000);                   // Wait 200 ms
}

// ******************************************************************************************
// *****        Get the Time & Date Information from the GPS Data in the Buffer         *****
// ******************************************************************************************
void getTimeDate(void)
{
        unsigned short  i = 0;                          // Define & initialize loop counter
        unsigned short  comma = 0;                      // Counter for Commas

        i = GPSstart;                                   // Assign position of start of '$GxRMC' ('$' position)

   if(GPS[i+6] == ',')
       {
        Hour    =           10*(GPS[i+7] - 48);         // 10's of Hours,   48 is ASCII Table offset
        Hour    = Hour  +      (GPS[i+8]  -48);         //         Hours,   48 is ASCII Table offset
        Min     =           10*(GPS[i+9]  -48);         // 10's of Minutes, 49 is ASCII Table offset
        Min     = Min  +       (GPS[i+10] -48);         //         Minutes, 48 is ASCII Table offset
        Sec     =           10*(GPS[i+11] -48);         // 10's of Seconds, 48 is ASCII Table offset
        Sec     = Sec  +       (GPS[i+12] -48);         //         Seconds, 48 is ASCII Table offset
       }

   for ( i = GPSstart; i < GPSstop; i++ )
       {
          if(GPS[i] == ',') comma = comma + 1;
             if(comma == 2)
               {
                  Valid =           GPS[i+1];           // Capture 'Data Valid' Flag
               }
             if(comma == 9)
               {
                   Day   =        10*(GPS[i+1]-48);     // 10's of Days,   48 is ASCII Table offset
                   Day   =  Day +    (GPS[i+2]-48);     //         Days,   48 is ASCII Table offset
                   Month =        10*(GPS[i+3]-48);     // 10's of Months, 48 is ASCII Table offset
                   Month =  Month +  (GPS[i+4]-48);     //         Months, 48 is ASCII Table offset
                   Year  =        10*(GPS[i+5]-48);     // 10's of Years,  48 is ASCII Table offset
                   Year  =   Year +  (GPS[i+6]-48);     //         Years,  48 is ASCII Table offset
                   break;                               // End the Loop
               }
       }
}

// ******************************************************************************************
// *****                            Display Time on LCD Screen                          *****
// ******************************************************************************************
void showTime(void)
{
                    short   tenHrs = Hour/10;           //  Define & calculate 10's of Hours
                    short   Hrs = Hour-10*tenHrs;       //  Define & calculate Hours
                    short   tenMin = Min/10;            //  Define & calculate 10's of Minutes
                    short   Mi = Min - 10*tenMin;       //  Define & calculate Minutes
                    short   tenSec = Sec/10;            //  Define & calculate 10's of Seconds
                    short   Sc = Sec - 10*tenSec;       //  Define & calculate Seconds

                    showChar((tenHrs+48), Pos[0]);      //  Show 10's of Hours
                    showChar((   Hrs+48), Pos[1]);      //  Show         Hours
                    showChar((tenMin+48), Pos[2]);      //  Show 10's of Minutes
                    showChar((    Mi+48), Pos[3]);      //  Show         Minutes
                    showChar((tenSec+48), Pos[4]);      //  Show 10's of Seconds
                    showChar((    Sc+48), Pos[5]);      //  Show         Seconds
                    LCDMEM[6]  |=  BIT2;                //  Turn   ':'   Symbol on
                    LCDMEM[19] |=  BIT2;                //  Turn   ':'   Symbol on
}

// ******************************************************************************************
// *****                            Display Date on LCD Screen                          *****
// ******************************************************************************************
void showDate(void)
{
                    short   tenMon = Month/10;          //  Define & calculate 10's of Months
                    short   Mon = Month - tenMon * 10;  //  Define & calculate         Months
                    short   tenDy  = Day/10;            //  Define & calculate 10's of Days
                    short   Dy  = Day   - tenDy * 10;   //  Define & calculate         Days
                    short   tenYr  = Year/10;           //  Define & calculate 10's of Years
                    short   Yr  = Year  - tenYr  * 10;  //  Define & calculate         Years

                    showChar((tenMon+48), Pos[0]);      //  Show 10's of Months
                    showChar((   Mon+48), Pos[1]);      //  Show         Months
                    showChar(( tenDy+48), Pos[2]);      //  Show 10's of Days
                    showChar((    Dy+48), Pos[3]);      //  Show         Days
                    showChar(( tenYr+48), Pos[4]);      //  Show 10's of Years
                    showChar((    Yr+48), Pos[5]);      //  Show         Years
                    LCDMEM[6]  |=  BIT2;                //  Turn   ':'   Symbol on
                    LCDMEM[19] |=  BIT2;                //  Turn   ':'   Symbol on
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
        // Show Underline for other character
        LCDMEM[position]   = 0x10;
        LCDMEM[position+1] = 0x00;
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
// Configure LFXT 32kHz CrystalOscillator
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
