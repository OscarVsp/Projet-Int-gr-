/*
 * File:   encoder.c
 * Author: L
 *
 * Created on 25 mars 2020, 15:18
 */


#include "xc.h"

#define PI 3.14159

void init_encoder(void){
    //Définition des inputs dans le multiplexeur
    RPINR14bits.QEA1R = 19; //Channel A dans le moteur 1
    RPINR14bits.QEB1R = 20; //Channel B dans le moteur 1
    QEI1CONbits.SWPAB = 1;
    QEI1CONbits.QEIM = 0b111; //QEI enable (4xmode) Counter reset by match(MAXxCNT)
    MAX1CNT = 65535;
    POS1CNT; //initialise le compteur de position du moteur 1
            
            
    RPINR16bits.QEA2R = 24; //Channel A dans le moteur 2
    RPINR16bits.QEB2R = 25; //Channel B dans le moteur 2
    //QEI2CONbits.SWPAB = 1;
    QEI2CONbits.QEIM = 0b111; //QEI enable (4xmode) Counter reset by match(MAXxCNT)
    MAX2CNT = 65535;
    POS2CNT; //initialise le compteur de position du moteur 2
}

float getValueEncoderL(void){
    float res;
    if (TRISBbits.TRISB4 == 0){
        res = ((POS1CNT)*(2*PI))/1440.0;        
    }
    else if (TRISBbits.TRISB4 == 1){
        res = -(((65535-(POS1CNT-1))*(2*PI))/1440.0);        
    }
    return res;
}

float getValueEncoderR(void){
    float res;
    if (TRISBbits.TRISB5 == 0){
        res = ((POS2CNT)*(2*PI))/1440.0;
        
    }
    else if (TRISBbits.TRISB5 == 1){
        res = -(((65535-(POS2CNT-1))*(2*PI))/1440.0);
  
    }
    return res;
}
