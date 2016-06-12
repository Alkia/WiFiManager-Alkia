#include "DHT.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <TimeLib.h> 
#include "globals.h"              // global structures and enums used by the application
///////////// Pin Set /////////////////
const int OLED_SCL_PIN = D1;
const int OLED_SDA_PIN = D2;
const int DHT11PIN = D3;          // Temperature sensor PIN set on NodeMCU  
const int analogInPin = A0;       // Numéro de la broche à laquelle est connecté la photorésistance
#define OLED_RESET LED_BUILTIN    // We don't have the reset on this OLED so put the LED_BUILTIN 
////////////// Constantes /////////////////

////////////// Global Objects /////////////
Adafruit_SSD1306 display(OLED_RESET); // The OLED driver
#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
////////////// Global Variables ///////////
SensorData data;
SensorData old_data;     
unsigned long previousMillisOLEDmessage = 10;  // To keep track of the time when when the previous message was displayed on the OLED
IPAddress timeServer; // time.nist.gov NTP server address
const char* ntpServerName = "time.nist.gov";
String messageOverlay = "";
String messageFrame2a = "";
String messageFrame2b = "";
String messageFrame2c = "";

void setup() {            // your setup code here, to run once 
  Serial.begin(115200);
  OLED_init();            // Start OLED and Clear the buffer.
  OLED_show_Alkia();      // Show ALKIA logo
  light_setup();           // Indique que la broche analogInPin est une entrée
  //WiFiManager - Local intialization. Once its business is done, there is no need to keep it around
  OLED_show_WiFi();
  WiFiManager wifiManager;
  messageOverlay = "WiFiManager Start";
  Serial.println(messageOverlay);
  OLED_scrolltext(messageOverlay);
  //  wifiManager.resetSettings();  //reset settings - for testing | This creates problmem on the long run
  wifiManager.setMinimumSignalQuality(25);  //  Eliminate WiFi with quality under 25%
  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  wifiManager.setTimeout(180);   //in seconds
  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if(!wifiManager.autoConnect("THE-BOX")) {
    OLED_show_Router();
    messageOverlay = "failed to connect and hit timeout";
    OLED_scrolltext(messageOverlay);
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
    OLED_scrolltext(messageOverlay);
    delay(15);
    messageOverlay = "WiFi.status :"+String(WiFi.status());
    Serial.println(messageOverlay );
    OLED_scrolltext(messageOverlay);    
  delay(15);
  WiFi.hostByName(ntpServerName, timeServer); 
  delay(15);
  messageOverlay = "Resolving NTP...  :)";
  Serial.println(messageOverlay );
  OLED_scrolltext(messageOverlay);
  delay(15);
  digitalClockDisplay(); // Display the time before refreshing NTP
  getCurrentTime(); // Refresh NTP
  delay(1000);
  ///// Initialize the Humidity and temperature data
  getDht11Readings(); // Read Humidity and temperature
  old_data = data;
  Serial.println("========================");
  OLED_show_Ok();
}

void loop() { // main code here, to run repeatedly:
  delay(1500);
  getDht11Readings();                // Read Humidity and temperature
  int lightintensity = light_read(); // Read Light
  Serial.println(" ");
  Serial.print("WiFi.status :"+String(WiFi.status())+" | IP:");
  Serial.println(WiFi.localIP());
  Serial.println("Light intensity: " + String(lightintensity));
  Serial.println(" Temperature:"+String(data.temperature)+" | Humidity:"+String(data.humidity)+" | Light:"+String(data.light));
  if (( data.temperature >= (old_data.temperature + 2)) || ( data.temperature <= (old_data.temperature - 2))){ // Temperature change
    messageOverlay = ">> Change in Temperature:"+String(data.temperature)+" |Before:"+String(data.temperature)+"|Light: " + String(lightintensity);
    Serial.println(messageOverlay );
    OLED_show_Light();
    OLED_displaytext(messageOverlay);      
    old_data = data; //Save the old data
  } 
   if ((data.humidity >= (old_data.humidity + 5)) || (data.humidity <= (old_data.humidity - 5))){ //  humidity change
    messageOverlay = ">> Change in Humidity:"+String(data.humidity)+" |Before:"+String(old_data.humidity)+"|Light: " + String(lightintensity);
    Serial.println(messageOverlay );
    OLED_show_Light();
    OLED_displaytext(messageOverlay);
  } 
  digitalClockDisplay(); // Display the time  
  messageFrame2a = hour() + formatDigits(minute()) + formatDigits(second());
  messageFrame2b = String(day()) + "." + String(month()) + "." + String(year()); 
  if (isItTimeToRefresh()) {
    delay(50);
    displayTempHumLight(data);
    }
  delay(50);
 // Serial.println("free Memory =");
}
