
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
