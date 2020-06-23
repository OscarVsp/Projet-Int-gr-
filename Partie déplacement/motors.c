/*
 * File:   motors.c
 * Author: L
 *
 * Created on 11 mars 2020, 15:09
 */


#include "xc.h"
#include "LIBPIC30.h"

void init_motors(void){
    //Définition des outputs
    TRISBbits.TRISB4 = 0; //dir 1
    TRISBbits.TRISB5 = 0; //dir 2
    LATBbits.LATB4 = 1;
    LATBbits.LATB5 = 1;
    
   
    RPOR6bits.RP13R = 0b10010;
    RPOR9bits.RP18R = 0b10011;
    
    //Définition du Timer 2
    PR2 = 4000;
    T2CONbits.TON = 1;
    
    OC1CONbits.OCTSEL = 0; //Timer2 is the clock source for compare 1
    OC1CONbits.OCM = 6; //PWM mode on OC1,Fault pin disabled
    
    OC2CONbits.OCTSEL = 0; //Timer2 is the clock source for compare 2
    OC2CONbits.OCM = 6; //PWM mode on OC1,Fault pin disabled
    
    OC1RS = 0;
    OC2RS = 0;
}

