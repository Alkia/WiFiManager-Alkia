#include <PubSubClient.h>         // MQTT
WiFiClient wclient;             // MQTT
PubSubClient client(server, mqttPort, callback, wclient);//your MQTT 
long lastMsg = 0;

// use this function to receive message from CloundMQTT subscribes topic
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
      digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
    }
}

void MQTT_setup()
{
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  //----MQTT validate connection ----//
  if (client.connect("arduinoClient", mqttUser, mqttPassword)) {
    Serial.println();
    Serial.println("MQTT Publish success.");
    client.publish(serialNumber,"NodeMCUconnected"); //Publish/send message to clound on outTopic
    client.subscribe(serialNumber); //Subscribes topic to receive message from cloud and print on monitor
    Serial.println("Subscribe success.");
    }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient", mqttUser, mqttPassword)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(serialNumber, "Reconnec");
      // ... and resubscribe
      client.subscribe(serialNumber);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void MQTT_heartbeat()
{
   if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    char msg[50];
    //// Send the IP address ////
    String str = String(WiFi.localIP());
    int str_len = str.length() + 1; 
    char IPchar_array[str_len];// Prepare the character array (the buffer) 
    str.toCharArray(IPchar_array, str_len);// Copy it over 
    //// END: Send the IP address ////
    snprintf (msg, 75, "Start SN=%s IP=%s", serialNumber,IPchar_array);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("heartbeat", msg);
  }
}

void MQTT_loop()
{
   if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    char msg[50];
    //snprintf (msg, 75, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(serialNumber, msg);
  }
}
