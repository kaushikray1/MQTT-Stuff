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

#include <ArduinoOTA.h>

void manage_WiFi()
{
  //This function is a part of the WiFi manager package
  //WiFiManager Local intialization of the wifi manager
  WiFiManager wifiManager;

  //reset Wi-Fi settings - if reset pin is pressed at startup
  delay(100);
  digitalRead(Reset_Pin);
  if(digitalRead(Reset_Pin) == LOW )
  {
    wifiManager.resetSettings();
    Serial.print("Reset Detected");
  }

  //give a static ip to the server
  //wifiManager.setSTAStaticIPConfig(IPAddress(192,168,4,1),IPAddress(192,168,4,1),IPAddress(255, 255, 255, 0));

  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  wifiManager.setTimeout(300);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("IoT_Setup")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
}
//------------------------------------------------------------------------------------------------------------------------------------------
void manage_OTA()
{
  ArduinoOTA.setHostname("LED-1");
  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
}
//------------------------------------------------------------------------------------------------------------------------------------------
void manage_housekeeping()
{
  //OTA Handeler 
  ArduinoOTA.handle();
  if (!client.connected())              reconnect();
  if (WiFi.status() != WL_CONNECTED)    manage_WiFi();
}
//------------------------------------------------------------------------------------------------------------------------------------------
