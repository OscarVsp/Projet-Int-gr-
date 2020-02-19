#include "xc.h"
#include "init.h"
#include "LIBPIC30.h"
#include "math.h"

void init_led(void){
    TRISBbits.TRISB10 = 0;
    TRISBbits.TRISB11 = 0;
    TRISBbits.TRISB12 = 0;
    TRISBbits.TRISB13 = 0;
    TRISBbits.TRISB14 = 0;
    clear_led();
}

void clear_led(void){
    LATBbits.LATB10 = 0;
    LATBbits.LATB11 = 0;
    LATBbits.LATB12 = 0;
    LATBbits.LATB13 = 0;
    LATBbits.LATB14 = 0;
}

void init_adc(void){
    AD1CON1bits.AD12B = 1;  // Convertisseur sur 12 bits
    AD1CON3bits.ADCS = 5;   // Diviseur d'horloge de l'adc
    AD1CON1bits.ASAM = 1;   // auto sample active
    AD1CSSLbits.CSS0 = 1;   // l'entree de l'ADC est AN0
    AD1PCFGLbits.PCFG0 = 0; // AN0 en mode analogique
    AD1CON1bits.ADON = 1;   // l'ADC est actif
    AD1CON1bits.SSRC = 2;   //Tringer sur la clock T3
}

void init_clock_smart(long freq){
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
    PR3 = 2500; 
    T3CONbits.TCKPS = 2;
}

void init_uart(){
    RPINR18bits.U1RXR = 7; //pour l'entrée, le multiplexeur est dans du périphérique uart, et on y met le numéro du pin
    RPOR3bits.RP6R = 3; //pour la sortie, le multiplexeur est dans le pin, et on y met le numéro du périphérique uart
    U1MODEbits.BRGH=0;
    U1BRG = (40000000)/(16*115200)-1;
    U1MODEbits.UARTEN = 1; //reception par défault
    U1STAbits.UTXEN = 1; //emission aussi (dans cet ordre))
}

void config_led(int ADC1BUF0){
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
    oscillatorInit();
    init_led();
    init_adc();
    init_uart();
    init_clock_smart(5000);
    while (1){
    int i = 0;
    while (U1STAbits.URXDA == 0){
                
    }
    U1RXREG;
    T3CONbits.TON = 1;
    while (i<20000) {
        if (IFS0bits.T3IF == 1){
            IFS0bits.T3IF = 0;
            while (!AD1CON1bits.DONE){
            
            }
            if (AD1CON1bits.DONE){
                while (U1STAbits.UTXBF==1){
                    
                }
                ADC1BUF0>>4;
                U1TXREG = ADC1BUF0;
            }
        i++;
        }    
        
    }
    T3CONbits.TON = 0;
    }
    return 0;
}
