#include <iostream>
#include <vector>
#include <string>
#include <algorithm> 
#include <Arduino.h>
#include "Ultrasonic.h"

using namespace std;

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
    int distance_mouvement;
    int historique[32];
  public:
    capteur_mouvement(int p){
      Pin=p;
      ultrasonic = new Ultrasonic(Pin); //allocation dans le tas (malloc + appel de constructeur)
      distance_mouvement = 9999;

    }
    int mesure_distance(){
      //distance=mesurer distance
      /*pinMode(Pin, OUTPUT);
      digitalWrite(Pin, HIGH);
      delayMicroseconds(10);
      digitalWrite(Pin, LOW);*/
      pinMode(Pin, INPUT);
      distance = ultrasonic->MeasureInCentimeters();
      return distance;
    }
    int scan (){
      distance_mouvement = ultrasonic->MeasureInCentimeters();
      cout << "distance mouvement :" << distance_mouvement << endl;
      return distance_mouvement;
    }
    bool detec_mouvement(){
      // true si un mouvement est détecté
      // false si pas de mouvement
      int distance_secur = ultrasonic->MeasureInCentimeters();

      cout << "distance securite :" << distance_secur << "distance_secur+0.1*distance_secur :" << distance_secur+0.1*distance_secur << "distance_mouvement :" << distance_mouvement << endl;

      if (((distance_secur+0.1*distance_secur) < distance_mouvement)){
        
        cout << "mouvement detecté"<< endl;
        return true;
      }
      else{
        return false;
      }
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
          int d = detecteur->mesure_distance();
          
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

