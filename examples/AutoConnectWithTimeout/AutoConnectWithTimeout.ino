#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
////////////// Constantes /////////////////

////////////// Global Variables ///////////
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
  delay(100);
  Serial.println("WiFi.status :"+String(WiFi.status()));
  delay(100);
  WiFi.hostByName(ntpServerName, timeServer); 
  Serial.println("Resolving NTP...  :)");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  Serial.println(" ");
  Serial.println("WiFi.status :"+String(WiFi.status())+" | IP:"+String(WiFi.localIP()));
}
