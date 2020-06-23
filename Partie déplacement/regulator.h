/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef REGULATOR_H
#define	REGULATOR_H

#include <xc.h>   


#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

void regTranslation(float longTot);
void translationLong(float commandeL);
void translationShort(float commandeL);

void regRotation(float angleTot);
void rotationLong(float commandeA);
void rotationShort(float commandeA);


#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* ENCODER_H */

