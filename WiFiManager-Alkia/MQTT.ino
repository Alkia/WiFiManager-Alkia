/*
 * by Maxime CARPENTIER
 * This adafuit library is satisfactory when MQTT_connect(); is in the setup phase and not in the declaration zone 
 * Connecting to MQTT... 
*/
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <ESP8266HTTPClient.h>
/************ GConstants ******************/
// Store the MQTT server, username, and password in flash memory.
// This is required for using the Alkia MQTT library.
 const char MQTT_SERVER[] PROGMEM    = AIO_SERVER;
 const char MQTT_USERNAME[] PROGMEM  = AIO_USERNAME;
 const char MQTT_PASSWORD[] PROGMEM  = AIO_KEY;
 const String TEMP = "45";  
 const String HMDT = "69";
 const String LIGT = "152";
 const String AIRQ = "120";
 const String SOND = "100";
 const String VIBT = "200";
/************ Global State ******************/
unsigned long currentTime;
unsigned long previousTime;
// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, AIO_SERVERPORT, MQTT_USERNAME, MQTT_PASSWORD);
/****************************** Feeds ***************************************/
// Setup a feed for publishing. Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish mqtt_publisher = Adafruit_MQTT_Publish(&mqtt, serialNumber);
// Setup a feed called for subscribing to changes.
Adafruit_MQTT_Subscribe mqtt_subscriber = Adafruit_MQTT_Subscribe(&mqtt, serialNumber);
/*************************** Get Fixed IP ************************************/
String GetFixedIP(){
        HTTPClient http;
        USE_SERIAL.print("[HTTP] begin...\n");       
        http.begin(FIXED_IP_URL);                        // configure traged server and url HTTPS    
        USE_SERIAL.print("[HTTP] GET...\n");         // start connection and send HTTP header
        int httpCode = http.GET();
        // httpCode will be negative on error
        if(httpCode > 0) {
            USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);             // HTTP header has been send and Server response header has been handled
            if(httpCode == HTTP_CODE_OK) {  // Answer found at server
                String payload = http.getString();
                USE_SERIAL.println(payload);
                String sIP  = payload.substring(7,21);
                int start = payload.indexOf(":\"") + 2; 
                int fin = payload.lastIndexOf("\"}");
                payload = payload.substring(start, fin);              
                USE_SERIAL.println(" start :" + String(start));   
                USE_SERIAL.println(" fin :" + String(fin));              
                USE_SERIAL.println(" Res :" + payload);
                http.end();
                return payload;
            }
        } else {
            USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
            http.end();
            return "";
        }
  }
/*************************** Setup ************************************/
void mqtt_setup() {
  USE_SERIAL.println("MQTT | WiFi connected");
  USE_SERIAL.println("MQTT | IP address: "); USE_SERIAL.println(WiFi.localIP());
  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&mqtt_subscriber);
  sIP = GetFixedIP();
  USE_SERIAL.print("MQTT | IP Address : " + sIP ); 
  // Setup MQTT subscription for onoff feed.
  MQTT_connect();
  mqtt.subscribe(&mqtt_subscriber);
  sIP = GetFixedIP();
  delay(5000);
}
/*************************** Loop Sample ************************************/
void mqtt_loop2() {
  String  message;
  // Ensure the connection to the MQTT server is alive (this will make the first connection and automatically reconnect when disconnected).  See the MQTT_connect function definition further below.
  MQTT_connect();
  // this is our 'wait for incoming subscription packets' busy subloop
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &mqtt_subscriber) {
      USE_SERIAL.print(F("Got: "));
      USE_SERIAL.println((char *)mqtt_subscriber.lastread);
    }
  }
  // Now we can publish stuff!
  USE_SERIAL.print(F("\nSending val "));
  USE_SERIAL.print("...");
  if (! mqtt_publisher.publish(message.c_str())) {
    USE_SERIAL.println(F("Failed"));
  } else {
    USE_SERIAL.println(F("OK!"));
  }
  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
}

// Function to send and receive to the MQTT server.
void mqtt_sendreceive(String  message) {
  USE_SERIAL.println("MQTT | message  : " + message);
  USE_SERIAL.println("MQTT | IP: " + sIP);
  // Ensure the connection to the MQTT server is alive (this will make the first connection and automatically reconnect when disconnected).  See the MQTT_connect function definition further below.
  MQTT_connect();
  // this is our 'wait for incoming subscription packets' busy subloop
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &mqtt_subscriber) {
      USE_SERIAL.print(F("MQTT Received | Got: "));
      USE_SERIAL.println((char *)mqtt_subscriber.lastread);
    }
  }
  // Now we can publish stuff!
  USE_SERIAL.print(F("\n MQTT | Sending val "));
  USE_SERIAL.print("...");
  if (! mqtt_publisher.publish(message.c_str())) {
    USE_SERIAL.println(F("Failed"));
  } else {
    USE_SERIAL.println(F("OK!"));
  }
  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;
  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }
  USE_SERIAL.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       USE_SERIAL.println(mqtt.connectErrorString(ret));
       USE_SERIAL.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         //while (1);
         USE_SERIAL.println("MQTT Cannot connect!");
         return;
       }
  }
  USE_SERIAL.println("MQTT Connected!");
  //delay(10000);
}

void updatetime(){
  previousTime = currentTime;
  currentTime = millis();
  USE_SERIAL.println("Time: " + String(currentTime) + " | Since last loop:" + String((currentTime-previousTime)/1000) ) + "Seconds ";
  }

