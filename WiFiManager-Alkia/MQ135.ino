/* Module AIR Quality with MQ135
 * https://hackaday.io/project/3475-sniffing-trinket/log/12363-mq135-arduino-library
 * https://github.com/GeorgK/MQ135
 * CONNECTIONS:
   - GND  - GND
   - +5V  - VCC
   - DOUT - D5
   - AOUT - A0
 * V1.00 04/08/2016
 * Author: maxime@xonnectedobjects.io
 */

//#include "MQ135.h"
//#define RZERO 76.63

void MQ135_init(){
pinMode (AIRQ_PIN, INPUT)  ;// analog  input signal (Not actually required; INPUT is default)
}

bool MQ135_read() {
  // lit la valeur du MQ135 et
  // stocke le résultat dans sensorValue :
  bool sensorValue = digitalRead(AIRQ_PIN);            
  // data.AIRQalert = sensorValue;  
  return sensorValue;  
}

// MQ135 gasSensor = MQ135(ANALOGPIN);

// Before you can use the sensor, it has to be calibrated. For this, connect the sensor to your circuit and leave it powered on for 12-24 h to burn it in. Then put it into outside air, preferably at 20°C/35% rel. hum. (humidity is not so crucial). Read out the calibration value as such
// float rzero = gasSensor.getRZero();
// Note: Different sensors will likely have different RZero!

// Congrats, you have calibrated the sensor and can now read the CO2 ppm value in your sketch

// float ppm = gasSensor.getPPM();

