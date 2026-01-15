#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm> 
#include <Arduino.h>
#include "Ultrasonic.h"

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

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

      cout << "distance securite :" << distance_secur <<endl << "distance_secur+0.1*distance_secur :" << distance_secur+0.1*distance_secur <<endl<< "distance_mouvement :" << distance_mouvement << endl;

      if (((distance_secur+0.3*distance_secur) < distance_mouvement)){
        
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
            cout << "Porte ouverte !" << endl;
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


// ------------- Partie notification -------------



class NotificationServeur : public Securite {
  private:
    AsyncWebServer server;
    AsyncEventSource events;
    const char* ssid;
    const char* password;

  public:
    // Constructeur : on lui passe les identifiants Wi-Fi
    NotificationServeur(const char* s, const char* p) 
        : server(80), events("/events"), ssid(s), password(p) {}

    void initialiser() {
        // 1. Connexion Wi-Fi
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);
        Serial.print("Connexion au Wi-Fi");
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }
        Serial.println("\nConnecté ! IP : " + WiFi.localIP().toString());

        // 2. Route pour la page HTML
        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
            String html = R"rawliteral(
                <!DOCTYPE html>
                <html>
                <head>
                    <title>Alarme INSA - Secu</title>
                    <meta name="viewport" content="width=device-width, initial-scale=1">
                    <style>
                        body { font-family: sans-serif; text-align: center; background: #222; color: white; }
                        #notifs { margin: 20px; padding: 10px; background: #333; border-radius: 8px; }
                        .msg { border-bottom: 1px solid #444; padding: 10px; color: #ff4444; font-weight: bold; }
                    </style>
                </head>
                <body>
                    <h2>🚨 Système de Sécurité</h2>
                    <div id="notifs">Historique des alertes...</div>
                    <script>
                        if (!!window.EventSource) {
                            var source = new EventSource('/events');
                            source.addEventListener('message', function(e) {
                                var node = document.createElement("div");
                                node.className = "msg";
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

        // 3. Lancer le service de notifications (SSE) et le serveur
        server.addHandler(&events);
        server.begin();
    }

    // La méthode que tu appelleras dans ton code principal
    void envoyer(String message) {
        events.send(message.c_str(), "message", millis());
        Serial.println("SSE envoyé : " + message);
    }
};