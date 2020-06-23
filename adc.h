/* 
 * File:   adc.h
 * Author: M. Osée
 *
 * Created on 11 janvier 2016, 14:10
 */

#ifndef ADC_H
#define	ADC_H


/**
 * En-tête de la librairie de l'ADC pour les labos d'ELEC-H310 (et ELEC-H309)
 * 
 * Ces fonctions utilisent l'ADC1 du dsPIC de l'Explorer16 pour convertir la 
 * l'entrée analogique AN0.
 * L'ADC1 est utilisé en polling, avec démarrage en software de la conversion
 */


/**
 * Initialise l'ADC1 pour convertir AN0, sur 12 bits, avec auto-sampling et 
 * démarrage en software
 */
void adcPollingInit(void);


/**
 * Démarre une conversion
 */
void adcPollingStart(void);

/**
 * renvoie 1 si la conversion est terminée, 0 sinon
 */
int adcPollingConversionFinished(void);
int adcConversionFinished(void);

/**
 * Renvoie le résultat de la dernière conversion.
 * Si on essaie de lire pendant une conversion, la fonction le détecte et 
 * renvoie -32768, qui est une valeur impossible comme résultat de conversion.
 */
int adcPollingRead(void);
int adcRead(void);


void adcTimerInit(void);


#endif	/* ADC_H */

