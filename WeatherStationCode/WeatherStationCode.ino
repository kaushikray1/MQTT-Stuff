#include <ESP8266WiFi.h>                    //https://github.com/esp8266/Arduino
//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>                    //https://github.com/tzapu/WiFiManager
#include <PubSubClient.h>
#include "DHT.h"

#define DHTPIN 2                            //what digital pin we're connecting DHT22
#define DHTTYPE DHT22                       // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);                   // Initialize DHT sensor.

#define Reset_Pin 0                         //pin to reset the wifi config files
IPAddress mqtt_server(192, 168, 1, 68);     //MQTT Broker IP
WiFiClient espClient;
PubSubClient client(espClient);
const char* MQTT_topic = "topic_name";

//------------------------------------------------------------------------------------------------------------------------------------------
void setup() {
  // put your setup code here, to run once:
  Serial.begin(250000);
  dht.begin();
  manage_WiFi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}
//------------------------------------------------------------------------------------------------------------------------------------------
void callback(char* topic, byte* payload, unsigned int length) {

  unsigned int red, green, blue;
  uint32_t rgb;
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
}
//------------------------------------------------------------------------------------------------------------------------------------------
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    //Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP", "username", "geforce4", "dev/test", 0, 0, "livingroom_LED Connected")) {
      //Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("dev/test", "Connected to MQTT");
      // ... and resubscribe
      client.subscribe(MQTT_topic);
    } else {
      //Serial.print("failed, rc=");
      //Serial.print(client.state());
      //Serial.println(" try again in 5 seconds");
      // Wait 3 seconds before retrying
      delay(2000);
    }
  }
}
//------------------------------------------------------------------------------------------------------------------------------------------
void loop() {
  char charVal[10];
  if (!client.connected())              reconnect();
  if (WiFi.status() != WL_CONNECTED)    manage_WiFi();
  
  float h = dht.readHumidity();
  dtostrf(h, 4, 4, charVal);
  client.publish("weather_H", charVal);
 
  float t = dht.readTemperature();         
  dtostrf(t, 4, 4, charVal);
  client.publish("weather_T", charVal);
  
  ESP.deepSleep(30 * 1000000, WAKE_RF_DEFAULT);
}
