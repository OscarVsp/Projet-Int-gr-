#include "init.h"
#include "adc.h"
#include "FskDetector.h"
#include "filtre.h"
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SEUIL 20
#define N 8
#define D (1<<N)

int32_t g900 = 0.006912819349439192*D;
int32_t g1100 = 0.008763313051597102*D;

int32_t coef900[4][6] = 
    {{1.0*D,  2.0*D,  1.0*D,  1.0*D,  -1.827445*D,    0.98717118*D},
    {1.0*D,   2.0*D,  1.0*D,  1.0*D,  -1.83171477*D,  0.98733*D},
    {1.0*D,  -2.0*D,  1.0*D,  1.0*D,  -1.83130553*D,  0.99461942*D},
    {1.0*D,  -2.0*D,  1.0*D,  1.0*D,  -1.84144489*D,  0.99477939*D}};

int32_t coef1100[4][6] =
    {{1.0*D,  2.0*D,  1.0*D,  1.0*D,  -1.74703422*D,  0.98374443*D},
    {1.0*D,   2.0*D,  1.0*D,  1.0*D,  -1.7535138*D,   0.98394705*D},
    {1.0*D,  -2.0*D,  1.0*D,  1.0*D,  -1.75076702*D,  0.993175*D},
    {1.0*D,  -2.0*D,  1.0*D,  1.0*D,  -1.76623042*D,  0.99337949*D}};

int main(void)
{   
    // Intitialisation de l'oscillateur
    oscillatorInit();

    adcTimerInit();
    
    uartInit();

    adcPollingStart();

    
    int16_t mes, detLow, detHigh;
    int32_t res1100, res900, max900 = 0, max1100 = 0;
    int msg;
    int i=0;
    while (1) {
        if(adcConversionFinished()){
            // Lit la valeur de l'ADC
            mes=adcRead();
            // Exectute les filtres
            res1100=filtre1100(mes);
            res900=filtre900(mes);
            i++;
            // Détection de max
            if(res1100>max1100){
                max1100=res1100;
            }
            if(res900>max900){
                max900=res900;
            }
        }
        if(i==70){
            i=0;

            detLow = (max900>SEUIL);
            detHigh = (max1100>SEUIL); 
            max900=0;
            max1100=0;
            msg=fskDetector(detLow, detHigh);
            // Envoie du message
            if(msg!=0){
                LATBbits.LATB6=1;
                while(U1STAbits.UTXBF){
                }
                U1TXREG=msg;
                while(U1STAbits.UTXBF){
                }
                U1TXREG=(msg>>8);
            }
        }
    }
    return 0;
}