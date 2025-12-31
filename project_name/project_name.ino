#include <ESP8266WiFi.h> 

const char* ssid     = "iPhone de Louayi";
const char* password = "Louayi38";

void setup() {
  Serial.begin(9600);
  delay(10);

  Serial.println();
  Serial.print("Connexion a : ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);


  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connecte");
  Serial.print("Adresse IP : ");
  Serial.println(WiFi.localIP()); 
}

void loop() {

}