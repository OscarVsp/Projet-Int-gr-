#include "xc.h"
#include "init.h"
#include "LIBPIC30.h"
#include "math.h"

void init_led(void){
    //Initialise les pins en output pour les led
    TRISBbits.TRISB10 = 0;
    TRISBbits.TRISB11 = 0;
    TRISBbits.TRISB12 = 0;
    TRISBbits.TRISB13 = 0;
    TRISBbits.TRISB14 = 0;
    clear_led();
}

void clear_led(void){
    //eteins toutes les leds
    LATBbits.LATB10 = 0;
    LATBbits.LATB11 = 0;
    LATBbits.LATB12 = 0;
    LATBbits.LATB13 = 0;
    LATBbits.LATB14 = 0;
}

void init_adc(void){
    //configuration de l'ADC
    AD1CON1bits.AD12B = 1;  // Convertisseur sur 12 bits
    AD1CON3bits.ADCS = 5;   // Diviseur d'horloge de l'adc
    AD1CON1bits.ASAM = 1;   // auto sample active
    AD1CSSLbits.CSS0 = 1;   // l'entree de l'ADC est AN0
    AD1PCFGLbits.PCFG0 = 0; // AN0 en mode analogique
    AD1CON1bits.ADON = 1;   // l'ADC est actif
    AD1CON1bits.SSRC = 2;   //Tringer sur la clock T3
}

void init_clock_smart(long freq){
    //configuration auto de la clock (attention aux overflows)
    long freq_base = 40000000;
    long scale = 0;
    while (freq_base/((pow(8,scale))*freq) > pow(2,16)){
        scale ++;
    }
    T3CONbits.TCKPS = scale;
    PR3 = freq_base/(pow(8,scale)*freq);
    T3CONbits.TON = 1;
}

void init_clock(void){
    //configuration clock pas automatique
    PR3 = 2500; 
    T3CONbits.TCKPS = 2;
}

void init_uart(){
    //configuration UART
    RPINR18bits.U1RXR = 7;  //pour l'entrée, le multiplexeur est dans du périphérique uart, et on y met le numéro du pin
    RPOR3bits.RP6R = 3;     //pour la sortie, le multiplexeur est dans le pin, et on y met le numéro du périphérique uart
    U1MODEbits.BRGH=0;      //mode de baud rate (fréquence de sur-échantillonnage)
    U1BRG = (40000000)/(16*115200)-1;
    U1MODEbits.UARTEN = 1;  //activation (reception uniquement par défault)
    U1STAbits.UTXEN = 1;    //emission aussi (dans cet ordre))
}

void config_led(int ADC1BUF0){
    //Actualise le bargraphe
    if (ADC1BUF0 > 683){
        LATBbits.LATB10 = 1;
        if (ADC1BUF0 > 1365){
            LATBbits.LATB11 = 1;
            if (ADC1BUF0 > 2048){
                LATBbits.LATB12 = 1;
                if (ADC1BUF0 > 2731){
                    LATBbits.LATB13 = 1;
                    if (ADC1BUF0 > 3413){
                        LATBbits.LATB14 = 1;
                    }
                }
            }
        }
    }
}

int main(void) {
    //initialisation de chaque composants
    oscillatorInit();
    init_led();
    init_adc();
    init_uart();
    init_clock_smart(1000);
    
    //main loop
    while (1){
        while (U1STAbits.URXDA == 0){   //attendre la réception de la donnée de l'ordinateur pour la synchro     
        }
        U1RXREG;                        //lecture de la donnée reçu pour clear le registre (on s'en fou de la donnée en soit ici)
        T3CONbits.TON = 1;              //activation de la clock
        
        int i = 0;                          //compteur pour faire 1000 mesures
        while (i<1000) {                    //loop pour faire les 1000 mesures
            if (IFS0bits.T3IF == 1){        //quand le flag de la clock trigger
                IFS0bits.T3IF = 0;          //On clear le flag
                while (!AD1CON1bits.DONE){  //On attend la fin de la conversion
                }
                if (AD1CON1bits.DONE){      
                    while (U1STAbits.UTXBF==1){ //On attend qu'il y ait de la place dans la file d'attente du UART
                    }
                    ADC1BUF0>>4;                //On shift de 4 bit vers la droit la mesure pour la passer de 12bits à 8 bits (perte de précision)
                    U1TXREG = ADC1BUF0;         //On copie la donnée de 8bits dans la file d'attente (sera envoyé automatiquement dés que possible)
                }
            i++;
            }    

        }
        T3CONbits.TON = 0; //Stop la clock
    }
    return 0;
}
