//**************************************************************
//************* Function void bme280RecorderBoot()**************
//**************************************************************
//
// This function steps thru the bootup process showing step by step on the serial monitor 
// and LCD the check our of I2C devices, LCD, wifi connectivity, date and time functions.
// If errors are encountered the process stops. I did this to get all this code out of
// void setup()
//
void bme280RecorderBoot() {

//Greeting
  
  Serial.println(F("***Hello!!!***"));
  Serial.println();
  Serial.println(F("I'm awake"));
  Serial.println();
  lcd.init();
  lcd.backlight();
  lcd.begin(screenWidth,screenHeight);     //initialize the lcd
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("     Hello!     "));
  lcd.setCursor(0, 1);
  lcd.print(F("   I'm awake"));
  delay(2000);

//
//Check BME 280 Sensors connectivity - if not connected-reboot
//
  Serial.println();
  Serial.println(F("Checking Multi Bosch BME280 Barometric Pressure-Humidity-Temp Sensors")); 

  if (!bme1.begin(0x76, &Wire1))
  {
   Serial.println(F("Could not find the First BME280 sensor - 0x76, check wiring!"));
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print(F("BME1 wiring error"));
   delay(3000);
   ESP.restart();  //infinite loop and crash, restart
  }
  if (!bme2.begin(0x77, &Wire1)) 
  {
   Serial.println(F("Could not find the Second BME280 sensor- 0x77, check wiring!"));
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print(F("BME2 wiring error"));
   delay(3000);
   ESP.restart();  //infinite loop and crash, restart 
  }

   Serial.println(F("BME-1 and BME-2 are connected")); 
   Serial.println();
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print(F("BME1 Responding"));
   lcd.setCursor(0, 1);
   lcd.print(F("BME2 Responding"));
   delay(2000);

  //Connect to WiFi and Report Status

  lcd.clear();
  lcd.setCursor(0, 0);

   checkWiFi();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi connected");
    lcd.setCursor(0,1); lcd.print("IP= "); lcd.print(WiFi.localIP());
    lcd.setCursor(0,2); lcd.print(WiFi.macAddress());
    lcd.setCursor(0,3); lcd.print("RSSI= "); lcd.print(WiFi.RSSI());

    delay(3000);
    
    lcd.clear();

//Display the current time from NTP Server

    Serial.println("Check time from the NTP Server");
    
    getNTC_Time(dateArr, numericalDate, timeF);
    
      // Send time and date to serial monitor
    Serial.print("");
    Serial.print("Time: "); Serial.println(timeF);
    Serial.print("Date: ");
    
      Serial.print(dateArrPtr);
      Serial.println();

    // Display time and date on the 20x4 LCD
    lcd.setCursor(0, 0);
    lcd.print(timeF);
    lcd.setCursor(0, 1);

      lcd.print(dateArrPtr);  
   
    delay(3000);
    
    lcd.clear();

     
  }

//**************************************************************
//**********END Function void bme280RecorderBoot()**************
//**************************************************************

/*
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation 
 * files (the "Software"), to deal in the Software without restriction, including without limitation the rights to 
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons 
 * to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


 */
