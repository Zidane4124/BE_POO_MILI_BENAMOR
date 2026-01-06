#include <iostream>
#include "Securite.cpp"
#include <Arduino.h>

using namespace std;
capteur_mouvement capteurMouv(D3);



void setup() {
    Serial.begin(115200);
    Serial.printf("\n Bonjour");
    
}



void loop(){

    delay (1000);
    cout << "distance :" << capteurMouv.detection_mouvement() << endl;

}