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

