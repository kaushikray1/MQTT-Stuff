#include <ArduinoOTA.h>

void manage_WiFi()
{
  pinMode(Reset_Pin, INPUT);
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  //reset settings - if GPIO 0 is pressed at startup
  delay(1000);
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
  if (!wifiManager.autoConnect("IoT_Sensor_Setup")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
}


void manage_OTA()
{
  // Hostname defaults to esp8266-[ChipID]

  ArduinoOTA.setHostname("Sta-1");

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
  //Serial.println("Ready");
  //Serial.print("IP address: ");
  //Serial.println(WiFi.localIP());
}

void manage_housekeeping()
{
  //OTA Handeler 
  ArduinoOTA.handle();
  if (!client.connected())              reconnect();
  if (WiFi.status() != WL_CONNECTED)    manage_WiFi();
}