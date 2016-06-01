#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
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
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  Serial.println("WiFiManager Start");
  //reset settings - for testing
  //wifiManager.resetSettings();
  //  Eliminate WiFi with quality under 25%
  wifiManager.setMinimumSignalQuality(25);
  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  wifiManager.setTimeout(180);
  
  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if(!wifiManager.autoConnect("THE-BOX")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  } 
  delay(100);
  //if you get here you have connected to the WiFi
  Serial.println("connected...  :)");
  delay(15);
  Serial.println("WiFi.status :"+String(WiFi.status()));
  delay(15);
  WiFi.hostByName(ntpServerName, timeServer); 
  delay(15);
  Serial.println("Resolving NTP...  :)");
  delay(15);
  digitalClockDisplay(); // Display the time before refreshing NTP
  getCurrentTime(); // Refresh NTP
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(2000);
  //Save the old data
  old_data = data;
  // Read Humidity and temperature
  getDht11Readings();
  Serial.println(" ");
  Serial.print("WiFi.status :"+String(WiFi.status())+" | IP:");
  Serial.println(WiFi.localIP());
  Serial.println(" Temperature:"+String(data.temperature)+" | Humidity:"+String(old_data.humidity));
  if ((old_data.temperature != data.temperature)){ // Temperature change
    Serial.println("Alert >> Change in Temperature:"+String(data.temperature)+" | Before:"+String(data.temperature));
  } 
   if ((old_data.humidity != data.humidity)){ //  humidity change
    Serial.println("Alert >> Change in Humidity:"+String(data.humidity)+" |Before:"+String(old_data.humidity));
  } 
  digitalClockDisplay(); // Display the time  
}
