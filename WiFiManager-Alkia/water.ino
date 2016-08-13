/*
 *  Test the water leak detection
 *  By maxime
 *  const int waterAnalogInPin = A0;       // Numéro de la broche analog à laquelle est connecté la hydrorésistance
 *  const int waterDigitalInPin = D4;       // Numéro de la broche digital à laquelle est connecté la hydrorésistance
*/

void water_setup() {
  pinMode(waterDigitalInPin, INPUT);     // Initialize the D8 pin as an input
 // pinMode(analogInPin, INPUT);   
}

// function retreive water digital
bool water_DigitalRead() {
  bool waterdigital = digitalRead(waterDigitalInPin);   // 
  return !waterdigital;                                 // invert the reading
}

// function retreive water analog
int water_AnalogRead() {
  int wateranalog = analogRead(analogInPin);
  return wateranalog;
}
