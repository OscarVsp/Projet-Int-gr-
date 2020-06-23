#include <xc.h>

//  CONFIGURATION BITS
///////////////////////////////////////////////////////////////////////////////
// FICD
#pragma config ICS = PGD1               // Comm Channel Select (Communicate on PGC1/EMUC1 and PGD1/EMUD1)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG is Disabled)
#pragma config FWDTEN = OFF

// The dsPIC will be clocked by the primary oscillator with a 10MHz crystal.
// We want to use the PLL to obtain Fosc = 80MHz ( <=> 40MIPS ).
// Problem : with a 10MHz crystal, PLL constraints are not met with the
// default parameter.
// Solution :
//	- boot using the internal FRC oscillator as clock source,
//	- set the right PLL parameters to obtain Fosc = 80MHz, without violating
//	  the PLL constraints,
//	- switch the clock source to the PLL
//	- wait for the end of the clock switch
//
#pragma config FNOSC = FRC              // Oscillator Mode (Internal Fast RC (FRC))
#pragma config POSCMD = XT              // Primary Oscillator Source (XT Oscillator Mode)
#pragma config OSCIOFNC = OFF           // OSC2 Pin Function (OSC2 pin has clock out function)
#pragma config FCKSM = CSECMD           // Clock Switching and Monitor (Clock switching is enabled, Fail-Safe Clock Monitor is disabled)

void oscillatorInit(void) {
    // Fosc = Fin*M/(N1+N2), where :
    //         N1 = PLLPRE + 2
    //         N2 = 2 x (PLLPOST + 1)
    //        M = PLLFBD + 2
    PLLFBD = 41;            // M = 32
    CLKDIVbits.PLLPRE = 0;    // N1 = 2
    CLKDIVbits.PLLPOST = 0;    // N2 = 2

 

    // Initiate Clock Switch to Primary Oscillator with PLL
    __builtin_write_OSCCONH( 1 );
    __builtin_write_OSCCONL( 1 );
    // Wait for Clock switch to occur
    while (OSCCONbits.COSC != 0b001);
}

void uartInit(void){
    RPINR18bits.U1RXR = 16; // Avoir la PIN 15 en reception 
    RPOR7bits.RP15R = 3; // PIN 14 en Output
    
    U1MODEbits.BRGH = 0; //Baudrate config 
    U1BRG =21 ;
    U1MODEbits.PDSEL = 00 ;//config format de données 
    U1MODEbits.UARTEN = 1;//Activer l'UART
    U1STAbits.UTXEN = 1;//Activer la transmission
}
