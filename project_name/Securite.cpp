#include <iostream>
#include <vector>
#include <string>
#include <algorithm> 
#include <Arduino.h>
#include "Ultrasonic.h"

class Securite {
  //classe mere
  public:
    virtual ~Securite() {}
};
class capteur_mouvement : public Securite{
  protected:
    int distance;
    int Pin;
    Ultrasonic *ultrasonic;
  public:
    capteur_mouvement(int p){
      Pin=p;
      ultrasonic = new Ultrasonic(Pin); //allocation dans le tas (malloc + appel de constructeur)
    }
    int detection_mouvement(){
      //distance=mesurer distance
      /*pinMode(Pin, OUTPUT);
      digitalWrite(Pin, HIGH);
      delayMicroseconds(10);
      digitalWrite(Pin, LOW);*/
      pinMode(Pin, INPUT);
      distance = ultrasonic->MeasureInCentimeters();
      return distance;
    }
};
  class Porte : public Securite {
    private:
        capteur_mouvement *detecteur; // la porte a un capteur
        bool etat; // true = ouverte; false = fermée

    public:
      Porte(int pinCapteur) {
          detecteur = new capteur_mouvement(pinCapteur);
          
      }
      ~Porte() { 
        delete detecteur; 
      }
      bool estOuverte() {
          int d = detecteur->detection_mouvement();
          
          if (d>1){
            etat = true;
          }
          else{
            etat = false;
          }
          //bool etat = (d > 1); // Si le capteur mesure plus d'un cm, la porte est ouiverte
          return etat;
      }
};
class Buzzer : public Securite {
  private:
    int PinBuzzer;
  public:
    Buzzer(int p){
      PinBuzzer=p;
    }
    void allumer(){
      digitalWrite(PinBuzzer, HIGH);
    }
    void eteindre(){
      digitalWrite(PinBuzzer, LOW);
    }
};

