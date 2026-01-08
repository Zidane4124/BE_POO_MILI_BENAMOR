#include <iostream>
#include "Securite.cpp"
#include <Arduino.h>

using namespace std;
capteur_mouvement capteurMouv(D3);
capteur_mouvement capteurMouv2(D6);
Buzzer Buzz (D5);
Porte porte(D3);


int etat = 0;
unsigned long last_time=0;
unsigned long last_time2=0;
unsigned long last_time3=0;
bool alarme=false;



void setup() {
    Serial.begin(115200);
    Serial.printf("\n Bonjour");
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
            
        }
        // pour test
        else{
            alarme=false;
            cout << "pas d'intrusion"<< endl;
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
            cout << "allumer !" << endl;
        }
        else{
            //eteindre
            Buzz.eteindre();
            cout << "eteindre !" << endl;
        }
        etat++;
        last_time = millis();
    }


    

}