const int gasPin = A0; //GAS sensor output pin to Arduino analog A0 pin

void airq_read()
{
  USE_SERIAL.println(analogRead(gasPin));
  delay(1000); // Print value every 1 sec.
}

