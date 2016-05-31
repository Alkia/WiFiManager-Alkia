#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
// library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
//#include <TimeLib.h>           // http://playground.arduino.cc/code/time - installed via library manager
// Constants
//#include "globals.h"        // global structures and enums used by the applocation
//IPAddress timeServer(203, 56, 27, 253); // NTP Server au.pool.ntp.org
// Variables
//CloudConfig cloud;
//DeviceConfig device;
//SensorData data;
//SensorData old_data;       

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();
  Serial.println("Start");

  //WiFiManager | Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //exit after config instead of connecting >>   //wifiManager.setBreakAfterConfig(true);
  //reset settings - for testing >>  //wifiManager.resetSettings();
  //  Eliminate WiFi with quality under 25%
  wifiManager.setMinimumSignalQuality(25);

  //tries to connect to last known settings
  //if it does not connect it starts an access point with the specified name
  //here  "THE-BOX" with password "password"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("THE-BOX", "password")) {
    Serial.println("failed to connect, we should reset as see if it connects");
    delay(3000);
    ESP.reset();
    delay(5000);
  }
  //if you get here you have connected to the WiFi
  Serial.println("connected! :-)");
  delay(500);
  Serial.println("local ip");
  Serial.println(WiFi.localIP());
  delay(100);
}

void loop() {
  delay(1000);
  Serial.println("My IP:"+WiFi.localIP());
}
