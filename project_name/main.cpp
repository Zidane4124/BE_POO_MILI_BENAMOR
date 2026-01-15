#include <iostream>
#include "Securite.h"
#include "server.h"
#include <Arduino.h>
#include <Servo.h>

using namespace std;
capteur_mouvement capteurMouv(D3); // porte
capteur_mouvement capteurMouv2(D6); // mouvement
Buzzer Buzz (D7);
Porte porte(D3, D5);
blink_led blink_1(D8);
blink_led blink_2(D4);
NotificationServeur iPhone("iPhone de Louayi", "Louayi38");
Servo servomoteur;


int etat = 0;
unsigned long last_time=0;
unsigned long last_time2=0;
unsigned long last_time3=0;
bool alarme=false;



void setup() {
    Serial.begin(115200);
    Serial.printf("\n Bonjour");

    iPhone.initialiser();
    capteurMouv2.scan();
    
    // Initialisation du scan initial pour le mouvement
    capteurMouv2.scan();


}



void loop(){


    //delay (1000);
    // Test du capteur de distance
    //cout << "distance :" << capteurMouv.detection_mouvement() << endl;
    // Test Porte
    if ((millis()-last_time2) > 1000){
        if ((porte.estOuverte()==true) || (capteurMouv2.detec_mouvement()==true)){

            alarme=true;
            cout << "intrusion"<< endl;
            last_time2 = millis();
            iPhone.envoyer("⚠️ INTRUSION DETECTEE !");
            
        }
        // pour test
        else{
            alarme=false;
            cout << "pas d'intrusion"<< endl;
            blink_1.eteindre();
            blink_2.eteindre();
            last_time2 = millis();
            etat = 0;
        }
    }
    /*if ((millis()-last_time3) > 1000){
        if (capteurMouv2.detec_mouvement()==true){
            alarme= true;
            cout << "intrusion"<<endl;
            last_time3 = millis();
        }
    }*/

    if ((alarme==true) && (etat < 20) && ((millis()-last_time) > 500)){
        if ((etat%2)==0){
            //allumer
            Buzz.allumer();
            blink_1.allumer();
            blink_2.eteindre();
            cout << "allumer !" << endl;
        }
        else{
            //eteindre
            Buzz.eteindre();
            blink_1.eteindre();
            blink_2.allumer();
            cout << "eteindre !" << endl;
        }
        etat++;
        last_time = millis();
    }


    

}