#include <FS.h>                   //this needs to be first, or it all crashes and burns...
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
//#include <Time.h> 
#include <TimeLib.h> 
#include "globals.h"        // global structures and enums used by the application
////////////// Constantes /////////////////

////////////// Global Variables ///////////
SensorData data;
SensorData old_data; 
IPAddress timeServer; // time.nist.gov NTP server address
const char* ntpServerName = "time.nist.gov";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();
  Serial.println("Start");
//  Serial.println("WiFi firmware:"+String(WiFi.firmwareVersion()));

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
 Serial.println("WiFiManager Start");
  wifiManager.setDebugOutput(true);
Serial.println("WiFiManager Debug");
  //exit after config instead of connecting
  wifiManager.setBreakAfterConfig(true);

  //reset settings - for testing
  //  wifiManager.resetSettings();

  //  Eliminate WiFi with quality under 25%
  wifiManager.setMinimumSignalQuality(25);

  // wifiManager.setConfigPortalTimeout()

  //tries to connect to last known settings
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP" with password "password"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("THE-BOX")) {
    Serial.println("failed to connect, we should reset as see if it connects");
    delay(3000);
    ESP.reset();
    delay(5000);
  }
  //if you get here you have connected to the WiFi
  Serial.println("connected...  :)");
  Serial.println("local ip : " + WiFi.localIP());
  //get a random server from the NTP pool
  Serial.println("WiFi.status :"+String(WiFi.status()));
  delay(1000);
  WiFi.hostByName(ntpServerName, timeServer); 
//  digitalClockDisplay(); // Display the time before refreshing NTP
//  getCurrentTime(); // Refresh NTP
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1500);
  //Save the old data
  old_data = data;
  // Read Humidity and temperature
  getDht11Readings();
  Serial.println("local ip");
  Serial.println(WiFi.localIP());
  Serial.println(" Temperature:"+String(data.temperature)+" | Humidity:"+String(old_data.humidity));
  if ((old_data.temperature != data.temperature)){ // Temperature change
    Serial.println("Warning >> Change in Temperature:"+String(data.temperature)+" | Before:"+String(data.temperature));
  } 
   if ((old_data.humidity != data.humidity)){ //  humidity change
    Serial.println("Warning >> Change in Humidity:"+String(data.humidity)+" |Before:"+String(old_data.humidity));
  } 
 // digitalClockDisplay(); 
}
