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
  // ---------- Classe : capteur_mouvement ----------
  // Héritage : Securite
  // Attributs :
  //  - distance : Entier stockant la dernière mesure brute en centimètres.
  //  - Pin : Numéro de la broche (GPIO) physique connectée au capteur.
  //  - *ultrasonic : Pointeur vers l'objet matériel de la bibliothèque Ultrasonic.
  //  - distance_mouvement : Distance de référence (ligne de base) établie lors du scan initial.
  //  - historique : Conteneur STL (std::vector) stockant les dernières mesures pour le filtrage.
  //  - MAX_SAMPLES : Constante définissant la taille de la fenêtre glissante pour la moyenne.
  //
  // Méthodes :
  //  - mesure_distance() : Effectue une lecture brute immédiate sur le capteur.
  //  - scan() : Calibre l'appareil en enregistrant la distance actuelle comme référence de repos.
  //  - detec_mouvement() : Compare la mesure actuelle au scan initial (avec marge de 50%). 
  //                        Lève une SecuriteException si le capteur ne répond pas.
  //  - mesure_distance_filtree() : Calcule la moyenne des mesures présentes dans 'historique' 
  //                                via l'algorithme STL std::accumulate pour éliminer le bruit.
  //
  // Description : 
  // Cette classe gère l'intelligence de la détection de présence. Elle transforme les 
  // signaux bruts du capteur HC-SR04 en données fiables grâce à un filtrage numérique 
  // (moyenne glissante). Elle assure la résilience du système en signalant les pannes 
  // via le mécanisme d'exceptions.
  // ------------------------------------------------
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
  // ---------- Classe : Loquet ----------
  // Héritage : Securite
  // Attributs :
  //  - pin : Entier représentant la pin utilisée pour piloter le servomoteur.
  //  - monServo : Objet de type Servo (bibliothèque Servo.h) permettant le contrôle angulaire.
  //  - verrouille : Booléen indiquant l'état logique actuel (true = verrouillé, false = déverrouillé).
  //
  // Méthodes :
  //  - Loquet(int p) : Constructeur qui initialise la broche, attache le servo et place 
  //                    le loquet en position "fermé" par défaut pour des raisons de sécurité.
  //  - ouvrir() : Positionne le servomoteur à 90° pour libérer le mécanisme et met à jour l'état.
  //  - fermer() : Positionne le servomoteur à 0° pour engager le verrou et met à jour l'état.
  //  - estVerrouille() : Accesseur (getter) permettant de consulter l'état du verrou.
  //
  // Description : 
  // Il est possible, à travers cette classe d'ouvrir/fermer la porte sans avoir à gérer l'angle du servomoteur.
  // Elle est utilisée par composition au sein de la classe Porte, illustrant la relation 
  // "Une Porte possède un Loquet".
  // -------------------------------------
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
    // ---------- Classe : Porte ----------
    // Héritage : Classe fille de Securite
    // Attributs :
    //  - detecteur : Pointeur vers un objet capteur_mouvement.
    //  - Lock : Pointeur vers un objet Loquet.
    //  - etat : Booléen représentant l'ouverture physique (true = ouverte, false = fermée).
    //
    // Méthodes :
    //  - Porte(int pinCapteur, int pinservo) : Constructeur qui instancie dynamiquement 
    //                                          le capteur et le loquet sur le Tas (Heap).
    //  - ~Porte() : Destructeur chargé de libérer la mémoire (delete) des composants.
    //  - estOuverte() : Interroge le détecteur ultrasonique pour mettre à jour et 
    //                   renvoyer l'état physique de la porte.
    //  - verrouiller() : Commande la fermeture du loquet. 
    //                    Lance une SecuriteException si la porte est ouverte.
    //  - deverrouiller() : Commande l'ouverture du loquet via l'objet Lock.
    //  - estVerrouillee() : Délègue à l'objet Lock la vérification de son état.
    //
    // Description : 
    // Cette classe se sert de la classe Loquet pour se verouiller/déverouiller.
    // Deux ascesseurs permettent de récuperer des attributs privés (etat et Lock->estVerrouille())
    // ------------------------------------
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
  // ---------- Classe : Buzzer ---------- 
  // Héritage : Classe fille de Securite 
  // Attributs : 
  // - PinBuzzer : Entier stockant le numéro de la broche GPIO.  
  // Méthodes : 
  // - Buzzer(int p) : Constructeur, configure la broche en mode SORTIE (OUTPUT). 
  // - allumer() : Active le signal électrique pour faire sonner le buzzer. 
  // - eteindre() : Coupe le signal électrique pour arrêter le son. 
  // Description : 
  // Cette classe gère l'alerte sonore du système. Elle simplifie l'usage de 
  // digitalWrite en proposant des méthodes explicites (allumer/eteindre). 
  // -------------------------------------
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
  // ---------- Classe : blink_led ---------- 
  // Héritage : Classe fille de Securite 
  // Attributs : 
  // - PinBlink_led : Entier stockant le numéro de la broche GPIO de la LED. 
  // Méthodes : 
  // - blink_led(int pin) : Constructeur, configure la broche en mode SORTIE (OUTPUT). 
  // - allumer() : Active la LED en envoyant un signal électrique. 
  // - eteindre() : Désactive la LED en coupant le signal électrique. 
  // Description : 
  // Cette classe gère la signalisation visuelle du système. Elle est utilisée 
  // dans le programme principal pour créer un effet de clignotement. 
  // -------------------------------------
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
  // ---------- Classe : stopAlarme ---------- 
  // Héritage : Classe fille de Securite 
  // Attributs : 
  // - _pin : Entier stockant le numéro de la broche GPIO du bouton. 
  // Méthodes : 
  // - stopAlarme(int pin) : Constructeur, configure la broche en mode ENTREE_PULLUP. 
  // - estAppuye() : Vérifie l'état électrique de la broche et renvoie vrai si pressé. 
  // Description : 
  // Cette classe gère la lecture des boutons de désactivation. L'utilisation du 
  // mode PULLUP permet de se passer de résistances externes lors du câblage. 
  // -----------------------------------------
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
  // ---------- Classe : NotificationServeur ---------- 
  // Héritage : Classe fille de Securite 
  // Attributs : 
  // - server : Serveur web asynchrone (port 80) pour l'interface utilisateur. 
  // - events : Flux d'événements (SSE) pour envoyer des messages en temps réel. 
  // - ssid / password : Identifiants de connexion au réseau Wi-Fi. 
  // Méthodes : 
  // - NotificationServeur(...) : Constructeur, initialise le serveur et le flux d'événements. 
  // - initialiser(Porte &laPorte) : Gère la connexion Wi-Fi et définit les routes HTML et de contrôle. 
  // - envoyer(String message) : Diffuse une notification texte vers l'interface web. 
  // Description : 
  // Cette classe gère la communication sans fil et l'interface mobile du système. 
  // Elle permet de piloter la porte à distance et d'afficher l'historique des alertes. 
  // ----------------------------------------------------
  private:
    AsyncWebServer server;
    AsyncEventSource events;
    const char* ssid;
    const char* password;
  public:
    NotificationServeur(const char* s, const char* p) 
        : server(80), events("/events"), ssid(s), password(p) {}
    void initialiser(Porte &laPorte) { 
        // --- Route API : Pilotage du loquet ---
        // On utilise une "Lambda function" [] pour capturer l'objet laPorte
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);
        // --- Route API : Pilotage du loquet ---
        // On utilise une "Lambda function" [] pour capturer l'objet laPorte
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