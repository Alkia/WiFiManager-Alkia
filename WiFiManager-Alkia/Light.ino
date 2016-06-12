/*
  Module Lit la luminosité captée par la photorésistance branchée sur la broche A0.
*/

void light_setup() {
  // Indique que la broche analogInPin est une entrée :
  pinMode(analogInPin, INPUT);     
}

int light_read() {
  // lit la valeur de la photorésistance et
  // stocke le résultat dans sensorValue :
  int sensorValue = analogRead(analogInPin);            
  // change sensorValue vers une intervalle de 0 à 255
  // et stocke le résultat dans outputValue :
 /// outputValue = map(sensorValue, 0, 1023, 0, 255);       
  // Debug Serial.println("\t output = " + String(sensorValue));   
  data.light = sensorValue;  
  return sensorValue;  
}
