
/*
#include <ESP8266WebServer.h>
#include <vector>
#include <string>
#include "Securite.h"

class GestionnaireWeb {
private:
    ESP8266WebServer* server;
    Porte* porte;
    capteur_mouvement* hall;
    Buzzer* buzz;

public:
    GestionnaireWeb(int port, Porte* p, capteur_mouvement* h, Buzzer* b) 
        : porte(p), hall(h), buzz(b) {
        server = new ESP8266WebServer(port);
    }

    // Initialisation des routes (URL)
    void setup() {
        // Route principale : affiche l'état et l'historique
        server->on("/", [this]() {
            String message = "<h1>Tableau de bord de Securite</h1>";
            
            // État actuel
            message += "<p>Porte : " + String(porte->estOuverte() ? "OUVERTE" : "FERMEE") + "</p>";
            
            // Utilisation des itérateurs pour l'historique (Partie 2 du TP)
            /*message += "<h3>Historique des intrusions (Logs STL) :</h3><ul>";
            
            // On suppose que la classe Buzzer a une méthode getLogs() renvoyant un vector
            std::vector<unsigned long> logs = buzz->getLogs(); 
            std::vector<unsigned long>::iterator it; // Déclaration de l'itérateur [cite: 45]
            
            // Parcours du conteneur avec begin() et end() [cite: 58, 59]
            for(it = logs.begin(); it != logs.end(); ++it) {
                message += "<li>Alarme declenchee a : " + String(*it) + " ms</li>";
            }
            message += "</ul>";*/
            
            /*server->send(200, "text/html", message);
        });

        server->begin();
        Serial.println("Serveur Web demarre !");
    }

    void actualiser() {
        server->handleClient();
    }
};*/