#include "DHT.h"                  // Library for the temperature & Humidity
#include <SPI.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <TimeLib.h> 
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino//needed for library
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DNSServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include "globals.h"              // global structures and enums used by the application
/*************************  Pin Set  *********************************/
#define OLED_RESET LED_BUILTIN    // We don't have the reset on this OLED so put the LED_BUILTIN 
const int OLED_SCL_PIN = D1;
const int OLED_SDA_PIN = D2;
const int DHT11PIN = D3;              // Temperature sensor PIN set on NodeMCU  
const int waterDigitalInPin = D4;     // Numéro de la broche digital à laquelle est connecté la hydrorésistance
const int AIRQ_PIN = D5;              // AirQuality  sensor warning PIN set on NodeMCU  
const int analogInPin = A0;           // Numéro de la broche à laquelle est connecté la photorésistance
const int waterAnalogInPin = A0;      // Not used! Numéro de la broche analog à laquelle est connecté la hydrorésistance
const int buttonPin = 0;          // NodeMCU pushbutton pin  // MQTT
const int ledPin =  16;           // LED pin on NodeMCU      // MQTT
/************************* WiFi Access Point *********************************/
#define USE_SERIAL Serial       // For debug
#define FIXED_IP_URL    "http://www.realip.info/api/p/realip.php"
/************************* Alkia.io Setup *********************************/
#define AIO_SERVER      "ec2-54-169-137-44.ap-southeast-1.compute.amazonaws.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    ""
#define AIO_KEY         ""
/************************* Constantes *********************************/
const int TIME_ZONE = 7;
const int LIGHT_OFFSET = 0;     // To take into account the physical setting [0 or 980]
const char* serialNumber = "BOX000005";            // MQTT used as the topic
const String mqttServer = "ec2-54-169-137-44.ap-southeast-1.compute.amazonaws.com";      // MQTT
const char* mqttUser = "aiibmjah";                  // MQTT
const char* mqttPassword = "xGRg9VsDPJVI";          // MQTT
const int  mqttPort =  1883 ;                       // MQTT
const int mqttSSLPort = 8883;                       // MQTT
const int mqttWebsocketsPortTLSonly = 9001;         // MQTT
const int mqttConnectionlimit = 10;                 // MQTT
// MQTT Update these with values suitable for your network.
IPAddress server(52,23,211,239); // CloudMQTT Server IP, ping m12.cloudmqtt.com to get 52.23.211.239
/* ---
const String mqttServer = "m12.cloudmqtt.com";      // MQTT
const char* mqttUser = "aiibmjah";                  // MQTT
const char* mqttPassword = "xGRg9VsDPJVI";          // MQTT
const int  mqttPort =  17987 ;                      // MQTT
const int mqttSSLPort = 27987;                      // MQTT
const int mqttWebsocketsPortTLSonly = 37987;        // MQTT
const int mqttConnectionlimit = 10;                 // MQTT

// MQTT Update these with values suitable for your network.
IPAddress server(52,23,211,239); // CloudMQTT Server IP, ping m12.cloudmqtt.com to get 52.23.211.239
*/
/************************* Global Objects *********************************/
Adafruit_SSD1306 display(OLED_RESET); // The OLED driver
#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
/************************* Global Variables *********************************/
SensorData data;
SensorData old_data;     
unsigned long previousMillisOLEDmessage = 10;  // To keep track of the time when when the previous message was displayed on the OLED
IPAddress timeServer; // time.nist.gov NTP server address
const char* ntpServerName = "time.nist.gov";
String messageOverlay = "";
String messageFrame2a = "";
String messageFrame2b = "";
String messageFrame2c = "";
String sIP = "";            // Used by MQTT to store the originating IP address 
/************************* water *********************************/
/************************* NTTP *********************************/
/************************* MQTT *********************************/
/************************* Setup *********************************/
void setup() {            // your setup code here, to run once 
  USE_SERIAL.begin(9600);
     // USE_SERIAL..setDebugOutput(true);
  messageOverlay = "Start";
  USE_SERIAL.println(" --- ");
  USE_SERIAL.println(messageOverlay);
  OLED_init();            // Start OLED and Clear the buffer.
  delay(100);
  messageOverlay = "OLED_show_Alkia";
  USE_SERIAL.println(messageOverlay);
  OLED_show_Alkia();       // Show ALKIA logo
  //WiFiManager - Local intialization. Once its business is done, there is no need to keep it around
  OLED_show_WiFi();
  WiFiManager wifiManager;
  messageOverlay = "WiFiManager Start";
  USE_SERIAL.println(messageOverlay);
  // OLED_scrolltext(messageOverlay);
  //  wifiManager.resetSettings();  //reset settings - for testing | This creates problmem on the long run
  wifiManager.setMinimumSignalQuality(25);  //  Eliminate WiFi with quality under 25%
  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  wifiManager.setTimeout(180);   //in seconds
  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  light_setup();           // Indique que la broche analogInPin est une entrée
  MQ135_init();            // Start the AIR Qality alert
  water_setup();           // Start the water alert 
  if(!wifiManager.autoConnect("THE-BOX")) {
    OLED_show_Router();
    messageOverlay = "failed to connect and hit timeout";
    OLED_scrolltext(messageOverlay);
    USE_SERIAL.println(messageOverlay );
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  } 
  delay(100);
  //if you get here you have connected to the WiFi
    messageOverlay = "connected...  :)";
    USE_SERIAL.println(messageOverlay );
    OLED_scrolltext(messageOverlay);
    delay(15);
    messageOverlay = "WiFi.status :"+String(WiFi.status());
    USE_SERIAL.println(messageOverlay );
    OLED_scrolltext(messageOverlay);    
  delay(15);
  WiFi.hostByName(ntpServerName, timeServer); 
  delay(15);
  messageOverlay = "Resolving NTP...  :)";
  USE_SERIAL.println(messageOverlay );
  OLED_scrolltext(messageOverlay);
  delay(15);
  digitalClockDisplay(); // Display the time before refreshing NTP
  getCurrentTime(); // Refresh NTP
  mqtt_setup();     // MQTT setup
  ///// Initialize the Humidity and temperature data
  getDht11Readings(); // Read Humidity and temperature
  old_data = data;
  USE_SERIAL.println("========================");
  OLED_show_Ok();
  OLED_show_bell();
  OLED_show_darkvador();
  OLED_show_biohazard();
  OLED_show_fire();
  OLED_show_eggs();
  OLED_show_heart();
  OLED_show_Humidity();
  OLED_show_temperature();
  OLED_show_dyno();
  OLED_show_protecteddatabase();
  OLED_show_reload();
  OLED_show_Sun();
  OLED_show_yingyang();
  OLED_show_nightday();
  OLED_show_parameters();
  OLED_show_cohiba();
  OLED_show_flood();
  OLED_show_flood2();
  OLED_show_gaz();
  OLED_show_crown();
}

void loop() { // main code here, to run repeatedly:
  delay(1500);
  getDht11Readings();               // Read Humidity and temperature
  bool AIRQ_alert = MQ135_read();                   // Read gaz alert
  bool WATR_alert = water_DigitalRead();            // Read the water alert
  int lightintensity = LIGHT_OFFSET - light_read(); // Read Light
  USE_SERIAL.println(" ");
  USE_SERIAL.print("WiFi.status :"+String(WiFi.status())+" | IP:");
  USE_SERIAL.println(WiFi.localIP());
  USE_SERIAL.println("Light intensity: " + String(lightintensity));
  USE_SERIAL.println(" Temperature:"+String(data.temperature)+" | Humidity:"+String(data.humidity)+" | Light:"+String(data.light));
  if (( data.temperature >= (old_data.temperature + 2)) || ( data.temperature <= (old_data.temperature - 2))){ // Temperature change
    messageOverlay = ">> Change in Temperature:"+String(data.temperature)+" |Before:"+String(data.temperature)+"|Light: " + String(lightintensity);
    USE_SERIAL.println(messageOverlay );
    OLED_show_Light();
    OLED_displaytext(messageOverlay);      
    old_data = data; //Save the old data
  } 
   if ((data.humidity >= (old_data.humidity + 5)) || (data.humidity <= (old_data.humidity - 5))){ //  humidity change
    messageOverlay = ">> Change in Humidity:"+String(data.humidity)+" |Before:"+String(old_data.humidity)+"|Light: " + String(lightintensity);
    USE_SERIAL.println(messageOverlay );
    OLED_show_Light();
    OLED_displaytext(messageOverlay);
  } 
  if (AIRQ_alert) {
      OLED_show_gaz();
    }
  if (WATR_alert) {
      OLED_show_flood2();
    }      
 // digitalClockDisplay(); // Display the time  
  messageFrame2a = hour() + formatDigits(minute()) + formatDigits(second());
  messageFrame2b = String(day()) + "." + String(month()) + "." + String(year()); 
  if (isItTimeToRefresh()) {
    delay(50);
    displayTempHumLight(data);
    }
  String msg = "{\"TEMP\":\""+String(data.temperature)+"\",\"HMDT\":\""+String(data.humidity)+"\",\"LIGT\":\""+String(data.light)+"\",\"AIRQ\":\""+String(data.AIRQ)+"\",\"SOND\":\""+String(data.SOND)+"\",\"VIBT\":\""+String(data.VIBT)+"\",\"AIRQ_Alert\":"+String(data.AIRQalert)+",\"WATR\":"+String(data.WATR)+",\"WATR_Alert\":"+String(data.WATRalert)+",\"IP\":\"" + sIP + "\"}"; // MQTT
  // USE_SERIAL.println("MQTT | Message to be sent:"+msg);
   mqtt_sendreceive(msg);
   updatetime();
}
