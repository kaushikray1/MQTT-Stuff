#include <ESP8266WiFi.h>                    //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>                    //https://github.com/tzapu/WiFiManager
#include <PubSubClient.h>
#include "DHT.h"
#include <ArduinoOTA.h>

#define DHTPIN 2                            //what digital pin we're connecting DHT22
#define DHTTYPE DHT22                       // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);                   // Initialize DHT sensor.

#define Reset_Pin 0                         //pin to reset the wifi config files
IPAddress mqtt_server(192, 168, 1, 68);     //MQTT Broker IP
WiFiClient espClient;
PubSubClient client(espClient);

//------------------------------------------------------------------------------------------------------------------------------------------
void setup() {
  // put your setup code here, to run once:
  Serial.begin(250000);
  dht.begin();
  manage_WiFi();
  //manage_OTA();
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
    if (client.connect("OutsideWeatherstation", "username", "geforce4", "dev/test", 0, 0, "WeatherStation Connected")) {
      //Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("dev/test", "Connected to MQTT");
      // ... and resubscribe
      //client.subscribe(MQTT_topic);
    } 
  }
}
//------------------------------------------------------------------------------------------------------------------------------------------
void loop() {
  char charVal[10];
  
  //OTA Handeler 
  //ArduinoOTA.handle();
  //housekeeping if any of the connections drop
  if (!client.connected())              reconnect();
  if (WiFi.status() != WL_CONNECTED)    manage_WiFi();
  
  float h = dht.readHumidity();
  dtostrf(h, 4, 4, charVal);
  client.publish("weather_H", charVal);
 
  float t = dht.readTemperature();         
  dtostrf(t, 4, 4, charVal);
  client.publish("weather_T", charVal,true);
  
  ESP.deepSleep(120 * 1000000, WAKE_RF_DEFAULT);
}
