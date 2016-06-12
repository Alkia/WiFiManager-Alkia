
// NaN (not a number) 
float dht11Calibration;
bool dht11Initialised = false;

DHT dht11(DHT11PIN, DHT11);

void initDHT11(){
  if (dht11Initialised) { return; }  
  dht11.begin();
  delay(150);  
  dht11Initialised = true;
}

void getDht11Readings(){
  int retryCount = 0;
  float temperature;
  float humidity; 
  initDHT11();
  
  do {  
    delay(150); // Good delay for this project
    //delay(200-retryCount*10);// gradually increase the delay
    temperature = dht11.readTemperature();
    humidity = dht11.readHumidity();
    if (!isnan(temperature)) { data.temperature = temperature; }  // Check if the is not NaN (not a number) and update the value
    if (!isnan(humidity)) {data.humidity = humidity;}             // Check if the is not NaN (not a number) and update the value
  } while ((isnan(temperature) || isnan(humidity)) && ++retryCount < 5);
}

