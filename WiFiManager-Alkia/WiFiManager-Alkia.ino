#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <TimeLib.h> 
#include "SSD1306.h"     /// For OLED Graphics
#include "SSD1306Ui.h"   /// For OLED Graphics
#include "globals.h"        // global structures and enums used by the application
///////////// Pin Set /////////////////
const int DHT11PIN = D3; //  on NodeMCU  
////////////// Constantes /////////////////

////////////// Global Variables ///////////
SensorData data;
SensorData old_data; 
IPAddress timeServer; // time.nist.gov NTP server address
const char* ntpServerName = "time.nist.gov";
String messageOverlay = "";
String messageFrame2a = "";
String messageFrame2b = "";
String messageFrame2c = "";
.
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  displaysetup(); // Start the graphics
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
    messageOverlay = "WiFiManager Start";
    Serial.println(messageOverlay );

  //reset settings - for testing | This creates problmem on the long run
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
    messageOverlay = "failed to connect and hit timeout";
    Serial.println(messageOverlay );
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  } 
  delay(100);
  //if you get here you have connected to the WiFi
    messageOverlay = "connected...  :)";
    Serial.println(messageOverlay );
    delay(15);
    messageOverlay = "WiFi.status :"+String(WiFi.status());
    Serial.println(messageOverlay );
  delay(15);
  WiFi.hostByName(ntpServerName, timeServer); 
  delay(15);
  messageOverlay = "Resolving NTP...  :)";
  Serial.println(messageOverlay );
  delay(15);
  digitalClockDisplay(); // Display the time before refreshing NTP
  getCurrentTime(); // Refresh NTP
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(15);
  //Save the old data
  old_data = data;
  // Read Humidity and temperature
  getDht11Readings();
  Serial.println(" ");
  Serial.print("WiFi.status :"+String(WiFi.status())+" | IP:");
  Serial.println(WiFi.localIP());
  Serial.println(" Temperature:"+String(data.temperature)+" | Humidity:"+String(data.humidity));
  if ((old_data.temperature != data.temperature)){ // Temperature change
    messageOverlay = "Alert >> Change in Temperature:"+String(data.temperature)+" | Before:"+String(data.temperature);
    Serial.println(messageOverlay );
  } 
   if ((old_data.humidity != data.humidity)){ //  humidity change

    messageOverlay = "Alert >> Change in Humidity:"+String(data.humidity)+" |Before:"+String(old_data.humidity);
    Serial.println(messageOverlay );
  } 
  digitalClockDisplay(); // Display the time  
  messageFrame2a = hour() + formatDigits(minute()) + formatDigits(second());
  messageFrame2b = String(day()) + "." + String(month()) + "." + String(year()); 
  loopGraphics();
}
