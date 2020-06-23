#ifndef FSKDETECTOR_H
#define	FSKDETECTOR_H


// Active le mode debug : le dsPIC envoie alors des messages d'erreur sur l'UART1
// voir testFskDetector3 pour un exemple d'utilisation
// #define DEBUG_FSK_DETECTOR
#include "xc.h"

#define MESSAGE_LENGTH      10                          // nombre de bits de donnees de la trame
#define SAMPLING_FREQ       800                         // frequence d'échantillonnage du signal audio, en Hz
#define BIT_FREQ            10                          // frequence des bits de la trame, en Hz
#define OSR                 (SAMPLING_FREQ/BIT_FREQ)    // OverSampling Ratio
#define FSK_MIN_SAMPLES_NB  (3*OSR/4)                   // nombre d'echantillons minimum pour considerer un bit valide

int16_t fskDetector(int16_t detLow, int16_t detHigh);


#endif	/* FSKDETECTOR_H */