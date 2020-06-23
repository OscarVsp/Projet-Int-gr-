/*
 * File:   regulator.c
 * Author: L
 *
 * Created on 18 mars 2020, 15:07
 */
#include "motors.h"
#include "encoder.h"
#include "xc.h"
#include "math.h"
#include "init.h"
#include "regulator.h"

#define RAYON  0.052
#define EMPATTEMENT 0.142
#define K_T 3.69 
#define K_R 2.33
#define VMAX 0.5 //On a choisi 0.5 m/s comme vitesse nominale
#define ACC 0.5 //On a choisi 0.5 m/s^2 comme acc�l�ration, ainsi on met 1s � atteindre la vitesse nominale

void regTranslation(float longTot){
    //On va v�rifier si on a le temps d'effectuer une acc�l�ration compl�te (pour atteindre la vitesse nominale)
    float t_accTot = VMAX/ACC;
    float t_acc = sqrt(fabs(longTot)/ACC);
    if (t_acc > t_accTot){//Si le temps d'acc�l�ration disponible est plus grand que celui n�cessaire
       translationLong(longTot);//On utilise le profil d'acc�l�ration trap�zo�dal
    }
    else {//Sinon
       translationShort(longTot);//On utilise le profil d'acc�l�ration triangulaire
    }
}


void translationShort(float commandeL){
    float Rval, Lval, longueur, angle, errorPOS, errorAngle, dcL, dcR;
    //D�finition de la direction de d�placement
    if (commandeL > 0){
        //Les roues tournent vers l'arri�re
        TRISBbits.TRISB4 = 0; 
        TRISBbits.TRISB5 = 0;
    }
    else {
        //Les roues tournent vers l'arri�re
        TRISBbits.TRISB4 = 1; 
        TRISBbits.TRISB5 = 1;
    }
    float com_abs = fabs(commandeL); //la valeur absolue de la consigne max est utilis�e pour �viter des 
                                     //probl�me dans la g�n�ration des consignes
    float acc = ACC;
    float t_acc = sqrt(com_abs/acc); //D�finition du temps d'acc�l�ration
    float consignelong;
    float consigneAngle = 0; //On veut une trajectoire en ligne droite
    int finished = 0; //Le robot est il arriv� � destination ? 0 = non/1 = oui
    int travel = 1; //Le robot � t'il fini de voyager ? (sa consigne � t'elle fini d'�voluer ?)
    float t = 0;
    //Boucle de r�gulation
    while(!finished){  //tant que le robot n'est pas arriv� � sa position finale, il continue
        if(IFS0bits.T3IF){ //Timer Flag (on veut une r�gulation � 100Hz)
            IFS0bits.T3IF = 0; //On remet le flag � z�ro
            t += 0.01; //on r�gule � 100Hz, donc on incr�mente de 0.01
            //D�finition de la consigne
            if (t<t_acc){
                consignelong = (acc/2)*pow(t,2);
            }
            else if (t<2*t_acc){
                consignelong = (com_abs/2)+(acc*t_acc)*(t-t_acc)-(acc/2)*pow((t-t_acc), 2);
            }
            else {
                consignelong = (com_abs);
                travel = 0;
            }
            if (commandeL < 0){
                consignelong = (-1)*consignelong;
            }
            //On va chercher les valeurs des encodeurs
            Lval = getValueEncoderL(); 
            Rval = getValueEncoderR(); 
            //On calcule la position (angulaire) du robot
            longueur = (Lval + Rval)*RAYON/2; //Ici c'est Lval + Rval car sinon on a des probl�mes de signe,
            angle = (Lval - Rval)*RAYON/EMPATTEMENT;  //par exemple on va vers la droite donc on veut une valeur 
                                                      //d'angle positive, mais Rval-Lval en renvoie une n�gative.
            //On calcule l'erreur sur la position du robot
            errorPOS = consignelong - longueur; 
            errorAngle = consigneAngle - angle; 
            //D�finition des rapports cycliques
            dcR = errorPOS*K_T - errorAngle*K_R; 
            dcL = errorPOS*K_T + errorAngle*K_R;
            //V�rification que le moteur gauche ne sature pas
            if (dcR < -1){
                dcR = -1;
            } else if (dcR > 1){
                dcR = 1;
            }
            //V�rification que le moteur droit ne sature pas
            if (dcL < -1){
                dcL = -1;
            } else if (dcL > 1){
                dcL = 1;
            }
            //R�gulation des moteurs
            OC1RS = fabs(dcL)*4000; 
            OC2RS = fabs(dcR)*4000; 
            //On v�rifie si il est arriv� � destination
            if ((com_abs - fabs(longueur) < 0.012) && !travel){
                finished = 1;
            }
        }
    }
    //On �teint les moteurs
    OC1RS = 0;
    OC2RS = 0;
}


void translationLong(float commandeL){
    float Rval, Lval, longueur, angle, errorPOS, errorAngle, dcL, dcR;
    //On d�fini le sens de parcours
    if (commandeL > 0){
        //Les roues tournent vers l'avant
        TRISBbits.TRISB4 = 0; 
        TRISBbits.TRISB5 = 0; 
    }
    else {
        //Les roues tournent vers l'arri�re
        TRISBbits.TRISB4 = 1; 
        TRISBbits.TRISB5 = 1; 
        
    }
    float com_abs = fabs(commandeL); //la valeur absolue de la consigne max est utilis�e pour �viter des 
                                     //probl�me dans la g�n�ration des consignes
    float acc = ACC;
    float vitesse = VMAX;
    float t_accTot = vitesse/acc; //temps n�cessaire � une acc�l�ration compl�te
    float l_accTot = (acc/2)*pow(t_accTot, 2);//longueur n�cessaire � une acc�l�ration compl�te
    float tTot = ((com_abs-2*l_accTot)/vitesse)+2*t_accTot; //temps total du parcours
    int finished = 0; //Le robot est il arriv� � destination ? 0 = non/1 = oui
    int travel = 1; //Le robot � t'il fini de voyager ? (sa consigne � t'elle fini d'�voluer ?)
    float t = 0; //temps �coul� depuis le d�but de la mise en route
    float consignelong;
    float consigneAngle = 0; //On veut une trajectoire en ligne droite
    
    //Boucle de r�gulation
    while(!finished){
        if(IFS0bits.T3IF){
            IFS0bits.T3IF = 0;
            t += 0.01;
            //D�termination de la consigne
            if (t<t_accTot){
                consignelong = (acc/2)*pow(t,2); //MRUA
            }
            else if (t<tTot-t_accTot){
                consignelong = l_accTot + (t-t_accTot)*vitesse; //MRU
            }
            else if (t<tTot){
                consignelong = (com_abs-l_accTot)+(t-tTot+t_accTot)*vitesse-(acc/2)*pow(t-tTot+t_accTot, 2); //MRUA
            }
            else {
                consignelong = com_abs;
                travel = 0;
            }
            if (commandeL < 0){
                consignelong = (-1)*consignelong;
            }
            //On va chercher les valeurs des encodeurs
            Rval = getValueEncoderR();
            Lval = getValueEncoderL(); 
            //Rval = getValueEncoderR(); 
            //On calcule la position (angulaire) du robot
            longueur = (Lval + Rval)*RAYON/2; 
            angle = (Lval - Rval)*RAYON/EMPATTEMENT;  
            //On calcule l'erreur sur la position (angulaire) du robot
            errorPOS = consignelong - longueur; 
            errorAngle = consigneAngle - angle; 
            //D�finition des rapports cycliques
            dcR = errorPOS*K_T - errorAngle*K_R; 
            dcL = errorPOS*K_T + errorAngle*K_R;
            //V�rification que le moteur gauche ne sature pas
            if (dcR < -1){
                dcR = -1;
            } else if (dcR > 1){
                dcR = 1;
            }
            //V�rification que le moteur droit ne sature pas
            if (dcL < -1){
                dcL = -1;
            } else if (dcL > 1){
                dcL = 1;
            }
            //R�gulation des moteurs
            OC1RS = fabs(dcL)*4000; 
            OC2RS = fabs(dcR)*4000; 
            //On v�rifie si il est arriv� � destination
            if (((com_abs - fabs(longueur)) < 0.015) && !travel){
                finished = 1;
            }
        }
    }
    //On �teint les moteurs
    OC1RS = 0;
    OC2RS = 0;
}

void regRotation(float angleTot){
    float t_accTot = VMAX/ACC;
    float t_acc = sqrt(fabs(angleTot)/ACC*EMPATTEMENT);
    if (t_acc > t_accTot){//Si le temps d'acc�l�ration disponible est plus grand que celui n�cessaire
       rotationLong(angleTot);//On utilise le profil d'acc�l�ration trap�zo�dal
    }
    else {//Sinon
       rotationShort(angleTot);//On utilise le profil d'acc�l�ration triangulaire
    }
}

void rotationShort(float commandeA){
    float Rval, Lval, longueur, angle, errorPOS, errorAngle, dcL, dcR;
    //D�finition de la direction de d�placement
    if (commandeA > 0){
        //Si on  veut tourner vers la droite
        TRISBbits.TRISB4 = 0; //La roue gauche tourne vers l'avant
        TRISBbits.TRISB5 = 1; //La route droite tourne vers l'arri�re
    }
    else {
        //Si on veut tourner vers la gauche
        TRISBbits.TRISB4 = 1; //La roue gauche tourne vers l'arri�re
        TRISBbits.TRISB5 = 0; //La roue gauche tourne vers l'avant
    }
    float com_abs = fabs(commandeA);//la valeur absolue de la consigne max est utilis�e pour �viter des 
                                    //probl�me dans la g�n�ration des consignes
    float acc = ACC/EMPATTEMENT; //On divise par l'empattement pour avoir l'acc�l�ration angulaire
    float t_acc = sqrt(com_abs/acc); //D�finition du temps d'acc�l�ration
    float consignelong = 0; //On veut qu'il reste sur place
    float consigneAngle; 
    int finished = 0; //Le robot est il arriv� � destination ? 0 = non/1 = oui
    int travel = 1; //Le robot � t'il fini de voyager ? (sa consigne � t'elle fini d'�voluer ?)
    float t = 0;
    //Boucle de r�gulation
    while(!finished){  //tant que le robot n'est pas arriv� � sa position finale, il continue
        if(IFS0bits.T3IF){ //Timer Flag (on veut une r�gulation � 100Hz)
            IFS0bits.T3IF = 0; //On remet le flag � z�ro
            t += 0.01; //on r�gule � 100Hz, donc on incr�mente de 0.01
            //D�finition de la consigne
            if (t<t_acc){
                consigneAngle = (acc/2)*pow(t,2);
            }
            else if (t<2*t_acc){
                consigneAngle = ((acc/2)*pow(t_acc,2))+((acc*t_acc)*(t-t_acc))-(acc/2)*pow((t-t_acc), 2);
            }
            else {
                consigneAngle = com_abs;
                travel = 0;
            }
            
            if (commandeA < 0){
                consigneAngle = (-1)*consigneAngle;
            }
            //On va chercher les valeurs des encodeurs
            Lval = getValueEncoderL(); 
            Rval = getValueEncoderR(); 
            //On calcule la position (angulaire) du robot
            longueur = (Lval + Rval)*(RAYON/2); 
            angle = (Lval - Rval)*(RAYON/EMPATTEMENT);  
            //On calcule l'erreur sur la position (angulaire) du robot
            errorPOS = consignelong - longueur; 
            errorAngle = consigneAngle - angle; 
            //D�finition des rapports cycliques
            dcL = errorPOS*K_T - errorAngle*K_R; 
            dcR = errorPOS*K_T + errorAngle*K_R;
            //V�rification que le moteur gauche ne sature pas
            if (dcR < -1){
                dcR = -1;
            } else if (dcR > 1){
                dcR = 1;
            }
            //V�rification que le moteur droit ne sature pas
            if (dcL < -1){
                dcL = -1;
            } else if (dcL > 1){
                dcL = 1;
            }
            //R�gulation des moteurs
            OC1RS = fabs(dcL)*4000; 
            OC2RS = fabs(dcR)*4000; 
            //On v�rifie si il est arriv� � destination
            if ((com_abs - fabs(angle) < 0.015) && !travel){
                finished = 1;
            }
        }
    }
    //On �teint les moteurs
    OC1RS = 0;
    OC2RS = 0;
}

void rotationLong(float commandeA){
    float Rval, Lval, longueur, angle, errorPOS, errorAngle, dcL, dcR;
    //On d�fini le sens de parcours
    if (commandeA > 0){
        //Si on veut tourner vers la droite
        TRISBbits.TRISB4 = 0; //La roue gauche tourne vers l'avant
        TRISBbits.TRISB5 = 1; //La roue droite tourne vers l'arri�re
    }
    else {
        //Si on veut tourner vers la gauche
        TRISBbits.TRISB4 = 1; //La roue gauche tourne vers l'arri�re
        TRISBbits.TRISB5 = 0; //La roue droite tourne vers l'avant
    }
    float com_abs = fabs(commandeA); //la valeur absolue de la consigne max est utilis�e pour �viter des 
                                     //probl�me dans la g�n�ration des consignes
    float acc = ACC/EMPATTEMENT; //On est en rotation donc rad/s^2
    float vitesse = VMAX/EMPATTEMENT; // rad/s
    float t_accTot = vitesse/acc; //temps n�cessaire � une acc�l�ration compl�te
    float l_accTot = (acc/2)*pow(t_accTot, 2);//longueur n�cessaire � une acc�l�ration compl�te
    float tTot = ((com_abs-2*l_accTot)/vitesse)+2*t_accTot; //temps total du parcours
    int finished = 0;//Le robot est il arriv� � destination ? 0 = non/1 = oui
    int travel = 1; //Le robot � t'il fini de voyager ? (sa consigne � t'elle fini d'�voluer ?)
    float t = 0; //temps �coul� depuis le d�but de la mise en route
    float consignelong = 0; //On veut qu'il reste sur place
    float consigneAngle; 
    
    
    //Boucle de r�gulation
    while(!finished){
        if(IFS0bits.T3IF){
            IFS0bits.T3IF = 0;
            t += 0.01;
            //D�termination de la consigne
            if (t<t_accTot){
                consigneAngle = (acc/2)*pow(t,2); //MRUA
            }
            else if (t<tTot-t_accTot){
                consigneAngle = l_accTot + (t-t_accTot)*vitesse; //MRU
            }
            else if (t<tTot){
                consigneAngle = (com_abs-l_accTot)+(t-tTot+t_accTot)*vitesse-(acc/2)*pow(t-tTot+t_accTot, 2); //MRUA
            }
            else {
                consigneAngle = com_abs;
                travel = 0;
            }
            if (commandeA < 0){
               consigneAngle = (-1)*consigneAngle;
            }
            //On va chercher les valeurs des encodeurs
            Lval = getValueEncoderL(); 
            Rval = getValueEncoderR(); 
            //On calcule la position (angulaire) du robot
            longueur = ((Lval + Rval)*RAYON)/2; 
            angle = ((Lval - Rval)*RAYON)/EMPATTEMENT;  
            //On calcule l'erreur sur la position (angulaire) du robot
            errorPOS = consignelong - longueur; 
            errorAngle = consigneAngle - angle; //Si angle est n�gatif /!\ car alors errorAngle != 0
            //D�finition des rapports cycliques
            dcR = errorPOS*K_T - errorAngle*K_R; 
            dcL = errorPOS*K_T + errorAngle*K_R;
            //V�rification que le moteur gauche ne sature pas
            if (dcR < -1){
                dcR = -1;
            } else if (dcR > 1){
                dcR = 1;
            }
            //V�rification que le moteur droit ne sature pas
            if (dcL < -1){
                dcL = -1;
            } else if (dcL > 1){
                dcL = 1;
            }
            //R�gulation des moteurs
            OC1RS = fabs(dcL)*4000; 
            OC2RS = fabs(dcR)*4000; 
            //On v�rifie si il est arriv� � destination et la consigne � fini d'�voluer
            if ((com_abs - fabs(angle) < 0.015) && !travel){
                finished = 1;
            }
        }
    }
    //On �teint les moteurs
    OC1RS = 0;
    OC2RS = 0;    
}