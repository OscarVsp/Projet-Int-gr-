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
#define ACC 0.5 //On a choisi 0.5 m/s^2 comme accélération, ainsi on met 1s à atteindre la vitesse nominale

void regTranslation(float longTot){
    //On va vérifier si on a le temps d'effectuer une accélération complète (pour atteindre la vitesse nominale)
    float t_accTot = VMAX/ACC;
    float t_acc = sqrt(fabs(longTot)/ACC);
    if (t_acc > t_accTot){//Si le temps d'accélération disponible est plus grand que celui nécessaire
       translationLong(longTot);//On utilise le profil d'accélération trapézoïdal
    }
    else {//Sinon
       translationShort(longTot);//On utilise le profil d'accélération triangulaire
    }
}


void translationShort(float commandeL){
    float Rval, Lval, longueur, angle, errorPOS, errorAngle, dcL, dcR;
    //Définition de la direction de déplacement
    if (commandeL > 0){
        //Les roues tournent vers l'arrière
        TRISBbits.TRISB4 = 0; 
        TRISBbits.TRISB5 = 0;
    }
    else {
        //Les roues tournent vers l'arrière
        TRISBbits.TRISB4 = 1; 
        TRISBbits.TRISB5 = 1;
    }
    float com_abs = fabs(commandeL); //la valeur absolue de la consigne max est utilisée pour éviter des 
                                     //problème dans la génération des consignes
    float acc = ACC;
    float t_acc = sqrt(com_abs/acc); //Définition du temps d'accélération
    float consignelong;
    float consigneAngle = 0; //On veut une trajectoire en ligne droite
    int finished = 0; //Le robot est il arrivé à destination ? 0 = non/1 = oui
    int travel = 1; //Le robot à t'il fini de voyager ? (sa consigne à t'elle fini d'évoluer ?)
    float t = 0;
    //Boucle de régulation
    while(!finished){  //tant que le robot n'est pas arrivé à sa position finale, il continue
        if(IFS0bits.T3IF){ //Timer Flag (on veut une régulation à 100Hz)
            IFS0bits.T3IF = 0; //On remet le flag à zéro
            t += 0.01; //on régule à 100Hz, donc on incrémente de 0.01
            //Définition de la consigne
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
            longueur = (Lval + Rval)*RAYON/2; //Ici c'est Lval + Rval car sinon on a des problèmes de signe,
            angle = (Lval - Rval)*RAYON/EMPATTEMENT;  //par exemple on va vers la droite donc on veut une valeur 
                                                      //d'angle positive, mais Rval-Lval en renvoie une négative.
            //On calcule l'erreur sur la position du robot
            errorPOS = consignelong - longueur; 
            errorAngle = consigneAngle - angle; 
            //Définition des rapports cycliques
            dcR = errorPOS*K_T - errorAngle*K_R; 
            dcL = errorPOS*K_T + errorAngle*K_R;
            //Vérification que le moteur gauche ne sature pas
            if (dcR < -1){
                dcR = -1;
            } else if (dcR > 1){
                dcR = 1;
            }
            //Vérification que le moteur droit ne sature pas
            if (dcL < -1){
                dcL = -1;
            } else if (dcL > 1){
                dcL = 1;
            }
            //Régulation des moteurs
            OC1RS = fabs(dcL)*4000; 
            OC2RS = fabs(dcR)*4000; 
            //On vérifie si il est arrivé à destination
            if ((com_abs - fabs(longueur) < 0.012) && !travel){
                finished = 1;
            }
        }
    }
    //On éteint les moteurs
    OC1RS = 0;
    OC2RS = 0;
}


void translationLong(float commandeL){
    float Rval, Lval, longueur, angle, errorPOS, errorAngle, dcL, dcR;
    //On défini le sens de parcours
    if (commandeL > 0){
        //Les roues tournent vers l'avant
        TRISBbits.TRISB4 = 0; 
        TRISBbits.TRISB5 = 0; 
    }
    else {
        //Les roues tournent vers l'arrière
        TRISBbits.TRISB4 = 1; 
        TRISBbits.TRISB5 = 1; 
        
    }
    float com_abs = fabs(commandeL); //la valeur absolue de la consigne max est utilisée pour éviter des 
                                     //problème dans la génération des consignes
    float acc = ACC;
    float vitesse = VMAX;
    float t_accTot = vitesse/acc; //temps nécessaire à une accélération complète
    float l_accTot = (acc/2)*pow(t_accTot, 2);//longueur nécessaire à une accélération complète
    float tTot = ((com_abs-2*l_accTot)/vitesse)+2*t_accTot; //temps total du parcours
    int finished = 0; //Le robot est il arrivé à destination ? 0 = non/1 = oui
    int travel = 1; //Le robot à t'il fini de voyager ? (sa consigne à t'elle fini d'évoluer ?)
    float t = 0; //temps écoulé depuis le début de la mise en route
    float consignelong;
    float consigneAngle = 0; //On veut une trajectoire en ligne droite
    
    //Boucle de régulation
    while(!finished){
        if(IFS0bits.T3IF){
            IFS0bits.T3IF = 0;
            t += 0.01;
            //Détermination de la consigne
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
            //Définition des rapports cycliques
            dcR = errorPOS*K_T - errorAngle*K_R; 
            dcL = errorPOS*K_T + errorAngle*K_R;
            //Vérification que le moteur gauche ne sature pas
            if (dcR < -1){
                dcR = -1;
            } else if (dcR > 1){
                dcR = 1;
            }
            //Vérification que le moteur droit ne sature pas
            if (dcL < -1){
                dcL = -1;
            } else if (dcL > 1){
                dcL = 1;
            }
            //Régulation des moteurs
            OC1RS = fabs(dcL)*4000; 
            OC2RS = fabs(dcR)*4000; 
            //On vérifie si il est arrivé à destination
            if (((com_abs - fabs(longueur)) < 0.015) && !travel){
                finished = 1;
            }
        }
    }
    //On éteint les moteurs
    OC1RS = 0;
    OC2RS = 0;
}

void regRotation(float angleTot){
    float t_accTot = VMAX/ACC;
    float t_acc = sqrt(fabs(angleTot)/ACC*EMPATTEMENT);
    if (t_acc > t_accTot){//Si le temps d'accélération disponible est plus grand que celui nécessaire
       rotationLong(angleTot);//On utilise le profil d'accélération trapézoïdal
    }
    else {//Sinon
       rotationShort(angleTot);//On utilise le profil d'accélération triangulaire
    }
}

void rotationShort(float commandeA){
    float Rval, Lval, longueur, angle, errorPOS, errorAngle, dcL, dcR;
    //Définition de la direction de déplacement
    if (commandeA > 0){
        //Si on  veut tourner vers la droite
        TRISBbits.TRISB4 = 0; //La roue gauche tourne vers l'avant
        TRISBbits.TRISB5 = 1; //La route droite tourne vers l'arrière
    }
    else {
        //Si on veut tourner vers la gauche
        TRISBbits.TRISB4 = 1; //La roue gauche tourne vers l'arrière
        TRISBbits.TRISB5 = 0; //La roue gauche tourne vers l'avant
    }
    float com_abs = fabs(commandeA);//la valeur absolue de la consigne max est utilisée pour éviter des 
                                    //problème dans la génération des consignes
    float acc = ACC/EMPATTEMENT; //On divise par l'empattement pour avoir l'accélération angulaire
    float t_acc = sqrt(com_abs/acc); //Définition du temps d'accélération
    float consignelong = 0; //On veut qu'il reste sur place
    float consigneAngle; 
    int finished = 0; //Le robot est il arrivé à destination ? 0 = non/1 = oui
    int travel = 1; //Le robot à t'il fini de voyager ? (sa consigne à t'elle fini d'évoluer ?)
    float t = 0;
    //Boucle de régulation
    while(!finished){  //tant que le robot n'est pas arrivé à sa position finale, il continue
        if(IFS0bits.T3IF){ //Timer Flag (on veut une régulation à 100Hz)
            IFS0bits.T3IF = 0; //On remet le flag à zéro
            t += 0.01; //on régule à 100Hz, donc on incrémente de 0.01
            //Définition de la consigne
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
            //Définition des rapports cycliques
            dcL = errorPOS*K_T - errorAngle*K_R; 
            dcR = errorPOS*K_T + errorAngle*K_R;
            //Vérification que le moteur gauche ne sature pas
            if (dcR < -1){
                dcR = -1;
            } else if (dcR > 1){
                dcR = 1;
            }
            //Vérification que le moteur droit ne sature pas
            if (dcL < -1){
                dcL = -1;
            } else if (dcL > 1){
                dcL = 1;
            }
            //Régulation des moteurs
            OC1RS = fabs(dcL)*4000; 
            OC2RS = fabs(dcR)*4000; 
            //On vérifie si il est arrivé à destination
            if ((com_abs - fabs(angle) < 0.015) && !travel){
                finished = 1;
            }
        }
    }
    //On éteint les moteurs
    OC1RS = 0;
    OC2RS = 0;
}

void rotationLong(float commandeA){
    float Rval, Lval, longueur, angle, errorPOS, errorAngle, dcL, dcR;
    //On défini le sens de parcours
    if (commandeA > 0){
        //Si on veut tourner vers la droite
        TRISBbits.TRISB4 = 0; //La roue gauche tourne vers l'avant
        TRISBbits.TRISB5 = 1; //La roue droite tourne vers l'arrière
    }
    else {
        //Si on veut tourner vers la gauche
        TRISBbits.TRISB4 = 1; //La roue gauche tourne vers l'arrière
        TRISBbits.TRISB5 = 0; //La roue droite tourne vers l'avant
    }
    float com_abs = fabs(commandeA); //la valeur absolue de la consigne max est utilisée pour éviter des 
                                     //problème dans la génération des consignes
    float acc = ACC/EMPATTEMENT; //On est en rotation donc rad/s^2
    float vitesse = VMAX/EMPATTEMENT; // rad/s
    float t_accTot = vitesse/acc; //temps nécessaire à une accélération complète
    float l_accTot = (acc/2)*pow(t_accTot, 2);//longueur nécessaire à une accélération complète
    float tTot = ((com_abs-2*l_accTot)/vitesse)+2*t_accTot; //temps total du parcours
    int finished = 0;//Le robot est il arrivé à destination ? 0 = non/1 = oui
    int travel = 1; //Le robot à t'il fini de voyager ? (sa consigne à t'elle fini d'évoluer ?)
    float t = 0; //temps écoulé depuis le début de la mise en route
    float consignelong = 0; //On veut qu'il reste sur place
    float consigneAngle; 
    
    
    //Boucle de régulation
    while(!finished){
        if(IFS0bits.T3IF){
            IFS0bits.T3IF = 0;
            t += 0.01;
            //Détermination de la consigne
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
            errorAngle = consigneAngle - angle; //Si angle est négatif /!\ car alors errorAngle != 0
            //Définition des rapports cycliques
            dcR = errorPOS*K_T - errorAngle*K_R; 
            dcL = errorPOS*K_T + errorAngle*K_R;
            //Vérification que le moteur gauche ne sature pas
            if (dcR < -1){
                dcR = -1;
            } else if (dcR > 1){
                dcR = 1;
            }
            //Vérification que le moteur droit ne sature pas
            if (dcL < -1){
                dcL = -1;
            } else if (dcL > 1){
                dcL = 1;
            }
            //Régulation des moteurs
            OC1RS = fabs(dcL)*4000; 
            OC2RS = fabs(dcR)*4000; 
            //On vérifie si il est arrivé à destination et la consigne à fini d'évoluer
            if ((com_abs - fabs(angle) < 0.015) && !travel){
                finished = 1;
            }
        }
    }
    //On éteint les moteurs
    OC1RS = 0;
    OC2RS = 0;    
}