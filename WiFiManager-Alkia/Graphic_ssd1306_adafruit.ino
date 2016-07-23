/*********************************************************************
Monochrome OLEDs based on SSD1306 drivers
This is for a 128x64 size display using I2C to communicate
3 pins are required to interface (2 I2C and one reset)
*********************************************************************/


//#define OLED_RESET LED_BUILTIN
//Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2
#define MAXMESSAGEDISPLAYDURATION 5000  // Max duration of an alert on the OLED

bool isItTimeToRefresh(){
  unsigned long currentMillis = millis();
  Serial.println("isItTimeToRefresh >> currentMillis="+String(currentMillis));
  Serial.println("isItTimeToRefresh >> previousMillisOLEDmessage="+String(previousMillisOLEDmessage));
  float delta = (float(currentMillis) - float(previousMillisOLEDmessage));
  //DEBUG:
  Serial.println("isItTimeToRefresh >> delta="+String(delta));
  if (delta >= MAXMESSAGEDISPLAYDURATION){
    previousMillisOLEDmessage = currentMillis;
    Serial.println("isItTimeToRefresh >> True ");
    display.clearDisplay();
    Serial.println("isItTimeToRefresh >> clearDisplay ");
    return true;
    } else { Serial.println("isItTimeToRefresh >> False"); return false; }
}

void displayTempHumLight(SensorData data){
  delay(50);
   Serial.println("displayTempHumLight >> OK");
  OLED_show_Ok();
  Serial.println("displayTempHumLight >> OK done");
  delay(50);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
   Serial.println("displayTempHumLight >> 1");
  display.clearDisplay();
   Serial.println("displayTempHumLight >> 2");
  display.println("Temp:" + String(round(data.temperature)));
  display.println("Humd:" + String(round(data.humidity)));
  display.println("Lgt :" + String(data.light));
  Serial.println("displayTempHumLight >> 3");
  Serial.println("currentTime Start" );
  String currentTime = String(day()) + "/" + String(month()) + "/" + String(year()) + " | " + String(hour()+TIME_ZONE);
  // String currentTime = String(hour()+TIME_ZONE);
  // String currentTime = String(hour()) + formatDigits(minute()) + formatDigits(second());
  Serial.println("currentTime" + currentTime);
  display.println(currentTime);
  display.display();
  Serial.println("displayTempHumLight >> end");
  }

void OLED_init(){
    // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
   // init done
   // Clear the buffer.
   display.clearDisplay();
   delay(10);
  // Show image buffer on the display hardware.
  // Clear the buffer Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(10);
  }

  void OLED_scrolltext(String sText) {
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.clearDisplay();
  display.println(sText);
  display.display();
  delay(1);
  display.startscrollright(0x00, 0x0F);
}

  void OLED_displaytext(String sText) {
  display.setTextSize(2);
  display.stopscroll();
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.clearDisplay();
  display.println(sText);
  display.display();
}

void OLED_show_WiFi(){
  display.clearDisplay();
  display.drawBitmap(32,0,WiFi_Logo,WiFi_Logo_height,WiFi_Logo_height,WHITE); // Show QR code Logo
  display.display();
  }

void OLED_show_Ok(){
  display.stopscroll();
  display.clearDisplay(); // Clear the buffer.
  display.drawBitmap(32,0,  ok_64px, ok_64px_height, ok_64px_width,WHITE); // Show QR code Logo
  display.display();
  delay(2000);
}

void OLED_show_Router(){
  display.stopscroll();
  display.clearDisplay();
  display.drawBitmap(32,0,router_64px, router_64px_height, router_64px_width,WHITE); // Show QR code Logo
  display.display();
  delay(2000);
}

void OLED_show_Signal(){
}
void OLED_show_Light(){
  display.stopscroll();
  display.clearDisplay(); //Clear the buffer.
  display.drawBitmap(32,0,light_64px, light_64px_height,light_64px_width,WHITE); // Show QR code Logo
  display.display();
  delay(2000);
}

void OLED_show_QR_alkia(){
  display.stopscroll();
  display.clearDisplay();//Clear the buffer.
  display.drawBitmap(32,0,  qrcode_alkia_net3, qrcode_alkia_net3_HEIGHT, qrcode_alkia_net3_WIDTH,WHITE); // Show QR code Logo
  display.display();
  delay(2000);
}

void OLED_show_Alkia(){
  display.stopscroll();
  display.clearDisplay();
  display.drawBitmap(32,0, AlkiaLogo64,64,64,WHITE); // Show Alkia Logo
  display.display();
  delay(1000);
  // invert the display
  display.invertDisplay(true);
  delay(1000); 
  display.invertDisplay(false);
  delay(1000); 
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.clearDisplay();
  display.println("Alkia");
  display.setTextSize(1);
  display.println("IT Services");
}

void testdrawbitmap(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  uint8_t icons[NUMFLAKES][3];
 
  // initialize
  for (uint8_t f=0; f< NUMFLAKES; f++) {
    icons[f][XPOS] = random(display.width());
    icons[f][YPOS] = 0;
    icons[f][DELTAY] = random(5) + 1;
    
    Serial.print("x: ");
    Serial.print(icons[f][XPOS], DEC);
    Serial.print(" y: ");
    Serial.print(icons[f][YPOS], DEC);
    Serial.print(" dy: ");
    Serial.println(icons[f][DELTAY], DEC);
  }

  while (1) {
    // draw each icon
    for (uint8_t f=0; f< NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, WHITE);
    }
    display.display();
    delay(200);
    
    // then erase it + move it
    for (uint8_t f=0; f< NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, BLACK);
      // move it
      icons[f][YPOS] += icons[f][DELTAY];
      // if its gone, reinit
      if (icons[f][YPOS] > display.height()) {
        icons[f][XPOS] = random(display.width());
        icons[f][YPOS] = 0;
        icons[f][DELTAY] = random(5) + 1;
      }
    }
   }
}

