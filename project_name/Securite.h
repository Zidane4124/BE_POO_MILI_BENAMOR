#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm> 
#include <Arduino.h>
#include "Ultrasonic.h"
#include <Servo.h>

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <numeric>
#include "exceptions.h"

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
    std::vector<int> historique;
    const size_t MAX_SAMPLES = 10;
    //int historique[32];
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
      if (distance_secur <= 0) {
        throw SecuriteException("Erreur Materielle : Capteur ultrason HS ou debranche");
      }
      cout << "distance securite :" << distance_secur <<endl << "distance_secur+0.1*distance_secur :" << distance_secur+0.5*distance_secur <<endl<< "distance_mouvement :" << distance_mouvement << endl;

      if (((distance_secur+0.5*distance_secur) < distance_mouvement)){
        
        cout << "mouvement detecté"<< endl;
        return true;
      }
      else{
        return false;
      }
    }
    int mesure_distance_filtree() { // on a fait cette fonction pour utiliser la STL
        int d = ultrasonic->MeasureInCentimeters();

        historique.push_back(d);

        if (historique.size() > MAX_SAMPLES) {
            historique.erase(historique.begin());
        }

        if (historique.empty()) return 0;
        int somme = std::accumulate(historique.begin(), historique.end(), 0);
        return somme / historique.size();
    }
};
class Loquet : public Securite {
  private:
    int pin;
    Servo monServo;
    bool verrouille;

  public:
    Loquet(int p) : pin(p), verrouille(true) {
        monServo.attach(pin);
        fermer(); // Position fermée par défaut
    }

    void ouvrir() {
        monServo.write(90); // Angle pour déverrouiller
        verrouille = false;
        cout << "Loquet déverrouillé" << endl;
    }

    void fermer() {
        monServo.write(0); // Angle pour verrouiller
        verrouille = true;
        cout << "Loquet verrouillé" << endl;
    }

    bool estVerrouille() { 
      return verrouille; 
      }
};
  class Porte : public Securite {
    private:
        capteur_mouvement *detecteur; // la porte a un capteur
        Loquet *Lock; // la porte a un loquet
        bool etat; // true = ouverte; false = fermée

    public:
      Porte(int pinCapteur,int pinservo) {
          detecteur = new capteur_mouvement(pinCapteur);
          Lock = new Loquet(pinservo);
          bool etat;
      }
      ~Porte() { 
        delete detecteur; 
        delete Lock;
      }
      bool estOuverte() {
          int d = detecteur->mesure_distance();
          
          if (d>1){
            etat = true;
            cout << "Porte ouverte !" << endl;
          }
          else{
            etat = false;
          }
          //bool etat = (d > 1); // Si le capteur mesure plus d'un cm, la porte est ouiverte
          return etat;
      }

      void verrouiller() { 
          if (this->estOuverte()) {
            throw SecuriteException("Securite : Impossible de verrouiller car la porte est ouverte");
          }
          Lock->fermer();
      }
      void deverrouiller() { 
          Lock->ouvrir(); 
      }
      bool estVerrouillee() { 
        return Lock->estVerrouille(); 
      }
};



class Buzzer : public Securite {
  private:
    int PinBuzzer;
  public:
    Buzzer(int p){
      PinBuzzer=p;
      pinMode(PinBuzzer, OUTPUT);
    }
    void allumer(){
      digitalWrite(PinBuzzer, HIGH);
    }
    void eteindre(){
      digitalWrite(PinBuzzer, LOW);
    }
};
class blink_led : public Securite {
  private:
  int PinBlink_led;

  public:
    blink_led(int pin){  //constructeur pour faire passer les variables D8 et D4
      PinBlink_led=pin;
      pinMode(pin, OUTPUT);
    }
    
   

  void eteindre(){
  digitalWrite(PinBlink_led, LOW);
  }

  void allumer(){
  digitalWrite(PinBlink_led, HIGH);
  }

};
class stopAlarme : public Securite {
  private:
  int _pin;
  
  public:
  stopAlarme(int pin){
    _pin = pin;
    pinMode(_pin, INPUT_PULLUP);
  }

    // Renvoie VRAI seulement si les DEUX boutons sont appuyés
    bool estAppuye(){
      if (digitalRead(_pin) == HIGH){
        return true; 
      }
      return false;
    }
};

// ------------- Partie notification -------------



class NotificationServeur : public Securite {
  private:
    AsyncWebServer server;
    AsyncEventSource events;
    const char* ssid;
    const char* password;

  public:
    NotificationServeur(const char* s, const char* p) 
        : server(80), events("/events"), ssid(s), password(p) {}

    void initialiser(Porte &laPorte) { 
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);
        Serial.print("Connexion au Wi-Fi");
        while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
        Serial.println("\nConnecté ! IP : " + WiFi.localIP().toString());

        server.on("/toggleLock", HTTP_GET, [&laPorte](AsyncWebServerRequest *request){
            if(laPorte.estVerrouillee()) {
                laPorte.deverrouiller();
                request->send(200, "text/plain", "OUVERT");
            } else {
                laPorte.verrouiller();
                request->send(200, "text/plain", "FERME");
            }
        });

        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
            String html = R"rawliteral(
                <!DOCTYPE html>
                <html>
                <head>
                    <title>Alarme INSA - Secu</title>
                    <meta name="viewport" content="width=device-width, initial-scale=1">
                    <style>
                        body { font-family: sans-serif; text-align: center; background: #222; color: white; }
                        .btn { padding: 20px; margin: 10px; font-size: 18px; border-radius: 10px; border: none; cursor: pointer; width: 80%; }
                        #lockBtn { background: #f39c12; color: white; font-weight: bold; }
                        #notifs { margin: 20px; padding: 10px; background: #333; border-radius: 8px; font-size: 0.9em; }
                    </style>
                </head>
                <body>
                    <h2>Systeme de Securite</h2>
                    <button id="lockBtn" class="btn" onclick="toggleLock()">CHANGER ETAT LOQUET</button>
                    <div id="notifs">Historique...</div>
                    <script>
                        function toggleLock() {
                            fetch('/toggleLock').then(response => response.text()).then(status => {
                                let btn = document.getElementById('lockBtn');
                                btn.innerHTML = (status == "OUVERT") ? "DEVERROUILLE" : "VERROUILLE";
                            });
                        }
                        if (!!window.EventSource) {
                            var source = new EventSource('/events');
                            source.addEventListener('message', function(e) {
                                var node = document.createElement("div");
                                node.style.color = "#ff4444";
                                node.innerHTML = "[" + new Date().toLocaleTimeString() + "] " + e.data;
                                document.getElementById("notifs").prepend(node);
                            }, false);
                        }
                    </script>
                </body>
                </html>
            )rawliteral";
            request->send(200, "text/html", html);
        });

        server.addHandler(&events);
        server.begin();
    }

    void envoyer(String message) {
        events.send(message.c_str(), "message", millis());
    }
};