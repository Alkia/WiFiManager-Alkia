#include "DHT.h"

float dht11Calibration;
bool dht11Initialised = false;


DHT dht11(DHT11PIN, DHT11);

void initDHT11(){
  if (dht11Initialised) { return; }
  
  dht11.begin();
  delay(100);  
  
  dht11Initialised = true;
}

void getDht11Readings(){
  int retryCount = 0;
  initDHT11();
  
  do {  
    delay(50);
    data.temperature = dht11.readTemperature(); 
    data.humidity = dht11.readHumidity();
  } while ((isnan(data.temperature) || isnan(data.humidity)) && ++retryCount < 10);
}

