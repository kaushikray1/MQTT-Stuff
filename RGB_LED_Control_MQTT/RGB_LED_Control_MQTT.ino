/*
*Code by Kaushik Ray
*/
/*
The MIT License

Copyright (c) by Kaushik Ray

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/


#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiManager.h> 
#include <ArduinoOTA.h>

// Update these with values suitable for your project
#define LED_RED     14
#define LED_GREEN   12
#define LED_BLUE    16
#define Reset_Pin    4                  //pin to reset the wifi config files

//MQTT Server Config
IPAddress mqtt_server(192, 168, 1, 68); //MQTT Broker IP
const char* Client_ID    = "testledb";
const char* username     = "username";
const char* password     = "geforce4";
const char* thing_name   = "thing_name";

WiFiClient espClient;
PubSubClient client(espClient);


//------------------------------------------------------------------------------------------------------------------------------------------
void setup() {

  pinMode(LED_RED,    OUTPUT);
  pinMode(LED_GREEN,  OUTPUT);
  pinMode(LED_BLUE,   OUTPUT);
  //pinMode(LED_BUILTIN, OUTPUT);
  pinMode(Reset_Pin, INPUT_PULLUP);

  digitalWrite(LED_RED,     0);
  digitalWrite(LED_GREEN,   0);
  digitalWrite(LED_BLUE,    0);

  analogWriteFreq(5000);
  analogWriteRange(255);

  Serial.begin(115200);
  Serial.println("Hello World");
  manage_WiFi();
  manage_OTA();
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
    if (client.connect(Client_ID, username, password, thing_name, 0, 0, Client_ID)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("dev/test", "Connected to MQTT");
      // ... and resubscribe
      client.subscribe(Client_ID);
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
