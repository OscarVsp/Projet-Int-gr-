/* 
 * File:   init.h
 * Author: mosee
 *
 * Created on 31 janvier 2020, 10:34
 */

#ifndef INIT_H
#define	INIT_H

#ifdef	__cplusplus
extern "C" {
#endif

    /* Configures PLL prescaler, PLL postscaler, PLL divisor to obtain 
     * Fosc = 80MHz (=> Fcy = 40MHz), with the XT oscillator using a 10MHz
     * crystal.*/
    void oscillatorInit(void);


#ifdef	__cplusplus
}
#endif

#endif	/* INIT_H */

