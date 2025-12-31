class Tache {
  public:
    Tache* suivante = nullptr; // Pointeur vers la tâche suivante (Liste Chaînée)
    
    // Le "contrat" : toutes les tâches doivent avoir cette fonction
    virtual void executer() = 0; 
};

class Ordonnanceur {
  private:
    Tache* premierWagon = nullptr;

  public:
    void ajouter(Tache* nouvelleTache) {
      //permet d'ajouter une tache a la liste chainée
      nouvelleTache->suivante = premierWagon;
      premierWagon = nouvelleTache;
    }

    void lancer() {
      // a partir de la toutes les taches vont s'enchainer
      Tache* enCours = premierWagon;
      while (enCours != nullptr) {
        enCours->executer(); 
        enCours = enCours->suivante;
      }
    }
};

// -------------------------------- Fonctions

#include <Servo.h>
#include <Ultrasonic.h> 

class TacheR2D2 : public Tache {
  private:
    Servo _servoTete;
    int _pinBuzzer;
    int _pinServo;
    unsigned long _lastActionTime = 0; // Pour le chrono interne

  public:
    TacheR2D2(int pinServo, int pinBuzzer) {
      // configurations des pins
      _pinServo = pinServo;
      _pinBuzzer = pinBuzzer;
      
      // Init du matériel
      _servoTete.attach(_pinServo);
      pinMode(_pinBuzzer, OUTPUT);
      _servoTete.write(90); // Tête droite au début
    }

    void executer() override { // code principal 
      
      if (millis() - _lastActionTime > 3000) { // 3 secondes à voir ( en gros toutes les 3 secondes il fait les instructions ici)

        _servoTete.write(45);

        tone(_pinBuzzer, 800, 100); 

        delay(200); 
        _servoTete.write(90);

        _lastActionTime = millis();
      }
    }
    
    void modePasContent() {
       tone(_pinBuzzer, 100, 500);
       _servoTete.write(0);
    }
};

class TacheCapteurs : public Tache { // cette classe permet de lire les valeurs des capteurs
  private:
    // objets a ajouter
    
  public:
    TacheCapteurs() {
      // initialisation des capteurs
    }

    void executer() override {

      static unsigned long lastRead = 0;
      
      if (millis() - lastRead > 100) { // on va lire un capteur toutes les 100ms
        
        lastRead = millis();
      }
    }
};

Ordonnanceur monRobot;

TacheR2D2     r2d2(D4, D5);   // Servo sur D4, Buzzer sur D5
TacheCapteurs surveillance;   // capteurs

void setup() {
  Serial.begin(9600);

  monRobot.ajouter(&r2d2);
  monRobot.ajouter(&surveillance);

}

void loop() {
  // lancement de la liste chainée
  monRobot.lancer();
}