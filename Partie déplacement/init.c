/*
 * File:   init.c
 * Author: L
 *
 * Created on 22 avril 2020, 14:51
 */


#include "xc.h"
#include "math.h"
#include "LIBPIC30.h"

#pragma config ICS = PGD1               // Comm Channel Select (Communicate on PGC1 and PGD)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG is Disabled)
#pragma config FWDTEN = OFF
#pragma config FNOSC = FRC              // Oscillator Mode (Internal Fast RC (FRC))
#pragma config POSCMD = HS              // Primary Oscillator Source (XT Oscillator Mode)
#pragma config OSCIOFNC = OFF           // OSC2 Pin Function (OSC2 pin has clock out function)
#pragma config FCKSM = CSECMD           // Clock Switching and Monitor (Clock switching is enabled, Fail-Safe Clock Monitor is disabled)

void oscillatorInit(void) {
	// Fosc = Fin*M/(N1+N2), where :
	// 		N1 = PLLPRE + 2
	// 		N2 = 2 x (PLLPOST + 1)
	//		M = PLLFBD + 2
    PLLFBD = 30;			// M = 32
    CLKDIVbits.PLLPRE = 0;	// N1 = 2
    CLKDIVbits.PLLPOST = 0;	// N2 = 2

	// Initiate Clock Switch to Primary Oscillator with PLL
	__builtin_write_OSCCONH( 3 );
	__builtin_write_OSCCONL( 1 );
	// Wait for Clock switch to occur
    while (OSCCONbits.COSC != 0b011);
}

void init_clock(void){
    
    T3CONbits.TCKPS = 1;
    PR3 = 24999;
    T3CONbits.TON = 1;
}
