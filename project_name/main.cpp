#include <iostream>
#include "Securite.cpp"
#include <Arduino.h>

using namespace std;
capteur_mouvement capteurMouv(D3);
Buzzer Buzz (D5);
Porte porte(D3);


int etat = 0;
unsigned long last_time=0;
bool alarme=false;


void setup() {
    Serial.begin(115200);
    Serial.printf("\n Bonjour");
}



void loop(){

    //delay (1000);
    // Test du capteur de distance
    //cout << "distance :" << capteurMouv.detection_mouvement() << endl;
    // Test Porte
    if ((porte.estOuverte()==true)){
        alarme=true;
        cout << "intrusion"<< endl;
    }
    // pour test
    else{
        alarme=false;
        cout << "pas d'intrusion"<< endl;
    }


    if ((alarme==true) && (etat < 20) && ((millis()-last_time) > 500)){
        if ((etat%2)==0){
            //allumer
            //Buzz.allumer();
            cout << "allumer !" << endl;
        }
        else{
            //eteindre
            //Buzz.eteindre();
            cout << "eteindre !" << endl;
        }
        etat++;
        last_time = millis();
    }

    

}