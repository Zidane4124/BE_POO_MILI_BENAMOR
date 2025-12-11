#include <ESP8266WiFi.h> // Bibliothèque native de l'ESP8266

// Remplacer par VOS identifiants de partage de connexion
const char* ssid     = "iPhone de Louayi";
const char* password = "Louayi38";

void setup() {
  Serial.begin(9600); // Vitesse du moniteur série
  delay(10);

  Serial.println();
  Serial.print("Connexion a : ");
  Serial.println(ssid);

  // Démarrage de la connexion
  WiFi.begin(ssid, password);

  // Boucle d'attente (tant que non connecté)
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Si on sort de la boucle, c'est connecté !
  Serial.println("");
  Serial.println("WiFi connecte !");
  Serial.print("Adresse IP : ");
  Serial.println(WiFi.localIP()); // Affiche l'adresse IP obtenue
}

void loop() {
  // Rien à faire ici pour le moment
}