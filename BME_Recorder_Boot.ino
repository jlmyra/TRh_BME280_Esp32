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

  // TFT greeting screen
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(80, 90);  // Centered position (approx)
  tft.setTextColor(TFT_CYAN);
  tft.setTextSize(3);
  tft.print(F("Hello!"));
  tft.setCursor(60, 130);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.print(F("I'm awake"));
  delay(2000);

//
//Check BME 280 Sensors connectivity - if not connected-reboot
//
  Serial.println();
  Serial.println(F("Checking Multi Bosch BME280 Barometric Pressure-Humidity-Temp Sensors"));

  // Reset watchdog before potentially long operation
  esp_task_wdt_reset();

  Serial.println(F("Attempting to initialize BME1 at 0x76..."));
  if (!bme1.begin(BME1_I2C_ADDRESS, &Wire))
  {
   Serial.println(F("Could not find the First BME280 sensor - 0x76, check wiring!"));
   tft.fillScreen(TFT_BLACK);
   tft.setCursor(10, 110);
   tft.setTextColor(TFT_RED);
   tft.setTextSize(2);
   tft.print(F("BME1 wiring error!"));
   delay(3000);
   ESP.restart();  //infinite loop and crash, restart
  }
  Serial.println(F("BME1 initialized successfully"));

  esp_task_wdt_reset();

  Serial.println(F("Attempting to initialize BME2 at 0x77..."));
  if (!bme2.begin(BME2_I2C_ADDRESS, &Wire))
  {
   Serial.println(F("Could not find the Second BME280 sensor- 0x77, check wiring!"));
   tft.fillScreen(TFT_BLACK);
   tft.setCursor(10, 110);
   tft.setTextColor(TFT_RED);
   tft.setTextSize(2);
   tft.print(F("BME2 wiring error!"));
   delay(3000);
   ESP.restart();  //infinite loop and crash, restart
  }
  Serial.println(F("BME2 initialized successfully"));

   Serial.println(F("BME-1 and BME-2 are connected"));
   Serial.println();
   tft.fillScreen(TFT_BLACK);
   tft.setCursor(10, 90);
   tft.setTextColor(TFT_GREEN);
   tft.setTextSize(2);
   tft.print(F("BME1 Responding"));
   tft.setCursor(10, 120);
   tft.print(F("BME2 Responding"));
   delay(2000);

  //Connect to WiFi and Report Status

  tft.fillScreen(TFT_BLACK);
  tft.setCursor(10, 110);
  tft.setTextColor(TFT_YELLOW);
  tft.setTextSize(2);
  tft.print(F("Connecting WiFi..."));

   Serial.println(F("Starting WiFi check..."));
   esp_task_wdt_reset();
   checkWiFi();
   Serial.println(F("WiFi check completed"));

    esp_task_wdt_reset();

    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN);
    tft.setTextSize(2);
    tft.setCursor(10, 30);
    tft.print(F("WiFi connected"));
    tft.setCursor(10, 60);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE);
    tft.print(F("IP: ")); tft.print(WiFi.localIP());
    tft.setCursor(10, 80);
    tft.print(WiFi.macAddress());
    tft.setCursor(10, 100);
    tft.print(F("RSSI: ")); tft.print(WiFi.RSSI());

    delay(3000);

    tft.fillScreen(TFT_BLACK);

//Display the current time from NTP Server

    Serial.println(F("==========================================="));
    Serial.println(F("Check time from the NTP Server"));
    Serial.println(F("This may take several seconds..."));
    esp_task_wdt_reset();

    getNTC_Time(dateArr, numericalDate, timeF);
    Serial.println(F("NTP time retrieved successfully"));

      // Send time and date to serial monitor
    Serial.print("");
    Serial.print("Time: "); Serial.println(timeF);
    Serial.print("Date: ");

      Serial.print(dateArrPtr);
      Serial.println();

    // Display time and date on the TFT
    tft.setTextColor(TFT_CYAN);
    tft.setTextSize(2);
    tft.setCursor(10, 90);
    tft.print(F("Time: "));
    tft.print(timeF);
    tft.setCursor(10, 120);
    tft.print(F("Date: "));
    tft.print(dateArrPtr);

    delay(3000);

    tft.fillScreen(TFT_BLACK);

//***************Initialize Google Sheets (after WiFi connected)********************

    Serial.println();
    Serial.println(F("==========================================="));
    Serial.println(F("*** [DIAGNOSTIC] Initializing Google Sheets ***"));
    Serial.println(F("WiFi is now connected - safe to initialize token"));
    Serial.println(F("==========================================="));

    // Set the callback for Google API access token generation status (for debug only)
    GSheet.setTokenCallback(tokenStatusCallback);

    // Set the seconds to refresh the auth token before expire (60 to 3540, default is 300 seconds)
    GSheet.setPrerefreshSeconds(10 * 60);

    Serial.println(F("*** [DIAGNOSTIC] Starting Google Sheets token generation ***"));
    Serial.println(F("Note: Token generation happens asynchronously in the background"));

    // Reset watchdog before potentially long operation
    esp_task_wdt_reset();

    // Begin the access token generation for Google API authentication
    // This is non-blocking and will initialize in the background
    GSheet.begin(CLIENT_EMAIL, PROJECT_ID, PRIVATE_KEY);

    Serial.println(F("*** [DIAGNOSTIC] Google Sheets begin() called ***"));
    Serial.println(F("Waiting for token to be ready (timeout: 60 seconds)..."));

    // Wait for token to be ready with timeout
    unsigned long tokenStartTime = millis();
    const unsigned long tokenTimeout = 60000; // 60 second timeout
    bool tokenReady = false;

    while ((millis() - tokenStartTime) < tokenTimeout && !tokenReady) {
      esp_task_wdt_reset(); // Keep watchdog happy

      // Service the library to allow token processing
      tokenReady = GSheet.ready();

      if (!tokenReady) {
        // Print progress every 2 seconds
        static unsigned long lastProgress = 0;
        if (millis() - lastProgress > 2000) {
          lastProgress = millis();
          Serial.print(F("*** [DIAGNOSTIC] Token still initializing... Elapsed: "));
          Serial.print((millis() - tokenStartTime) / 1000);
          Serial.println(F(" seconds ***"));
        }
        delay(100); // Small delay to prevent tight loop
      } else {
        Serial.println(F("*** [DIAGNOSTIC] Token initialization SUCCESS! ***"));
        Serial.print(F("*** Time taken: "));
        Serial.print((millis() - tokenStartTime) / 1000);
        Serial.println(F(" seconds ***"));
      }
    }

    if (!tokenReady) {
      Serial.println();
      Serial.println(F("###################################################"));
      Serial.println(F("!!! WARNING: Token did not become ready in time !!!"));
      Serial.println(F("###################################################"));
      Serial.println(F("Token will continue initializing in background"));
      Serial.println(F("Google Sheets updates will start once ready"));
      Serial.println();
    }

    // Reset watchdog after operation
    esp_task_wdt_reset();

    Serial.println();

//***************End Google Sheets Initialization********************

    Serial.println(F("==========================================="));
    Serial.println(F("Boot sequence completed successfully!"));
    Serial.println(F("Entering main loop..."));
    Serial.println(F("==========================================="));
    esp_task_wdt_reset();

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
