/*
Code by Kaushik Ray

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiManager.h> 
#include <ArduinoOTA.h>

// Update these with values suitable for your network.
#define LED_RED     14
#define LED_GREEN   12
#define LED_BLUE    16

const char* ssid = "Ray";
const char* password = "ambidextrous2014";
IPAddress mqtt_server(192, 168, 1, 68); //MQTT Broker IP

WiFiClient espClient;
PubSubClient client(espClient);

#define Reset_Pin 0                         //pin to reset the wifi config files

//------------------------------------------------------------------------------------------------------------------------------------------
void setup() {

  pinMode(LED_RED,    OUTPUT);
  pinMode(LED_GREEN,  OUTPUT);
  pinMode(LED_BLUE,   OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_RED,     0);
  digitalWrite(LED_GREEN,   0);
  digitalWrite(LED_BLUE,    0);

  analogWriteFreq(5000);
  analogWriteRange(255);

  Serial.begin(115200);
  manage_OTA();
  //setup_wifi();
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

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1')
    rgb = 0xFFFFFF;
  if ((char)payload[0] == '0')
    rgb = 0x000000;
  if ((char)payload[0] == '#') // we get RGB data
    rgb = (uint32_t) strtol((const char *) &payload[1], NULL, 16);

  for (int i = 0; i < length; i++) {
    payload[i]=0xFF;
  }

  red   = ((rgb >> 16) & 0xFF);
  green = ((rgb >>  8) & 0xFF);
  blue  = ((rgb >>  0) & 0xFF);

  analogWrite(LED_RED,     red);
  analogWrite(LED_GREEN, green);
  analogWrite(LED_BLUE,   blue);

}
//------------------------------------------------------------------------------------------------------------------------------------------
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("livingroom_LED1", "username", "geforce4", "dev/test", 0, 0, "livingroom_LED Connected")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("dev/test", "Connected to MQTT");
      // ... and resubscribe
      client.subscribe("livingroom_LED");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 2 seconds before retrying
      delay(2000);
    }
  }
}
//------------------------------------------------------------------------------------------------------------------------------------------
void loop() 
{
  manage_housekeeping();
  client.loop();
}
