/*
 * File:   newmainXC16.c
 * Author: L
 *
 * Created on 4 mars 2020, 16:19
 */


#include "xc.h" //librairie de base
#include "motors.h"
#include "encoder.h"
#include "regulator.h"
#include "init.h"



int main(void) {
    oscillatorInit();
    init_clock();
    init_motors();
    init_encoder();
    //regRotation(2);
    regTranslation(-1.5);
    while(1){
        
    }
}
