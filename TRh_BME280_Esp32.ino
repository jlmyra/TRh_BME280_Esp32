//*******************************************************************************************




//*********Please see Licenses and Recognition in file xxx_License***************

//**********################****NOTES****#################**********

//Requires UNCOMMENTING DateStrings.cpp Line 22 #include <string.h> // for strcpy_P or strcpy
//to compile!

//Create a secrets.h file that includes these items:

  //#define SECRET_myChannelNumber XXXXX; -ThingSpeak Channel Number
  //#define SECRET_myWriteAPIKey "XXXXXXXXXXXXXXXX"; - ThingSpeak write API Key
  
  //#define SECRET_ssid "YOUR WiFi SSID";
  //#define SECRET_password "YOUR WiFi Password";

  //#define SECRET_latitude "local latitude";
  //#define SECRET_longitude "local longitude";

  //This sketch makes extensive use of char arrays (strings) instead of using the String library in an effort to minimize the 
  //impact on the heap memory of the alphanumeric content displayed on the LsCD 

//**********################****NOTES****################***********

//*******************************************************************************************
#include <Arduino.h>
#include "secrets.h"     //See NOTES Above
#include "config.h"      // Configuration constants

//*********************** WATCHDOG TIMER **************************
#include "esp_task_wdt.h"

#define WDT_TIMEOUT 30  // Watchdog timeout in seconds

//*********************** BOOT COUNTER **************************
// RTC memory variable to count boots (persists across software resets)
RTC_DATA_ATTR int bootCount = 0;

//*********************** WIFI SETTINGS **************************

#include <WiFi.h> //for ESP32 removed //#include <ESP8266WiFi.h>
//#include <WiFiClient.h> //for Thingspeak

WiFiClient client; //for Thingspeak

const char *ssid = SECRET_ssid;
const char *password = SECRET_password;
int x = 0; 

//***********************End WIFI SETTINGS **************************

//*********************** ThinkSpeak SETTINGS **************************
 
#include "ThingSpeak.h"   //Thingspeak

unsigned long myChannelNumber = SECRET_myChannelNumber;  
const char *myWriteAPIKey = SECRET_myWriteAPIKey; 
String api_key = SECRET_api_key;

//***********************End ThinkSpeak SETTINGS **************************

//*********************** Google Sheets SETTINGS **************************
#include <ESP_Google_Sheet_Client.h>
#include <GS_SDHelper.h>
//#define PROJECT_ID "trh-recorder"
const char *PROJECT_ID = SECRET_PROJECT_ID;

//#define CLIENT_EMAIL "trh-recorder@trh-recorder.iam.gserviceaccount.com"
const char *CLIENT_EMAIL = SECRET_CLIENT_EMAIL;

#include <GS_SDHelper.h>

const char PRIVATE_KEY[] PROGMEM = SECRET_PRIVATE_KEY PROGMEM;

//const char spreadsheetId[] = "1Z33oKb0VU3ibr2prLX-wPez9gaddVGn8gYhAURCZLe0";  //from spreadsheet URL
const char spreadsheetId[] = SECRET_spreadsheetId;

unsigned long lastTime = 0;
unsigned long timerDelay = 30000;

//*********************** END Google Sheets SETTINGS ************************

//*********************NTC Clock Current Time**********************

#include <WiFiUdp.h>
#include <NTPClient.h> // Include NTPClient library
#include <Time.h>
#include <Timezone.h>

// Define NTP properties
#define NTP_OFFSET   60 * 60      // In seconds
#define NTP_INTERVAL 60 * 1000    // In miliseconds
#define NTP_ADDRESS  "north-america.pool.ntp.org"  // change this to whatever pool is closest (see ntp.org)
#define TIMEZONE usET // See NTP_Time.h tab for other "Zone references", UK, usMT etc

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);
  char formattedTime[37];
  char numericalDate[12]; char *numericalDatePtr = numericalDate;
  char dateArr[24]; char *dateArrPtr = dateArr;
  char timeF[11]; char *timeFPtr = timeF;

  const char commaSp[3] = ", "; const char *commaSpPtr = commaSp;
  const char *dayName[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
  const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "June", "July", "Aug", "Sep", "Oct", "Nov", "Dec"} ;
  const char *monthNum[] = {"01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12"} ;
  const char *dayNum[] = {"01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", 
                          "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31"};                    
  char hourNum[3]; char *hourNumPtr = hourNum;  // hour integer stored in this array
  char minuteNum[3]; char *minuteNumPtr = minuteNum;  // minute integer stored in this array 
  char yearNum[5]; char *yearNumPtr = yearNum;
  const char *ampm[] = {"AM", "PM"};  const char **ampmPtr = ampm;
  
//*********************NTC Clock Current Time**********************


//*********************** BME280 ***********************
#include <Wire.h>    // imports the wire library for I2C 
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// Create two BME280 instances
/*  
 *  BME 280 Connections
 *  0x76  CSB to V+ (for I2C mode) and SDO to GND
 *  0x77  CSB to V+ (for I2C mode) and SDO to V+
 *  https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BME280-DS002.pdf
 */
  Adafruit_BME280 bme1; //Adafruit_BME280 bme1(0x76); // I2C
  Adafruit_BME280 bme2; //Adafruit_BME280 bme2(0x77); // I2C 

  float tempCorr = SECRET_tempCorr;
  float rhCorr = SECRET_rhCorr;

//*********************LCD*********************
  #include <hd44780.h>            // main hd44780 header - see: https://github.com/duinoWitchery/hd44780
  #include <hd44780ioClass/hd44780_I2Cexp.h>  // i2c LCD i/o class header

  // LCD_COLS and LCD_ROWS are defined in config.h
  hd44780_I2Cexp lcd;

  int screenWidth = 20;   //LCD Display
  int screenHeight = 4;   //LCD Display

//*********************LCD*********************


//*********************I2C*********************
  //This sketch uses 2 I2C Channels. One for the LCD display and one for the BME280's
  //I2C WIRE default pins SCL - 22, SDA - 21

  #define SDA_2 33  //Set Pins for I2c WIRE1
  #define SCL_2 32

//*********************I2C*********************


//**********Array Variables********************

//This sketch minimizes the use of Strings and instead uses string character arrays to minimize the impact
//on the memory heap - which is particularly important with an ESP8266. So, everything is read into arrays
//and array pointers are used to move data into the arrays. It's complicated but it works!


//LCD Row 0: Date & Time

  char row0[77]; char *row0Ptr = row0;  //Text for scrolling lcd Row 0
  uint16_t old_row0StrLen = 0;
  uint16_t new_row0StrLen = 0;

  uint16_t stringStart, stringStop = 0; //Initialize Row 0 Scrolling Letter Counter
  int scrollCursor = screenWidth;  //Initialize Row 0 Scrolling Letter variable

//LCD Row 1: Sensor Data BME21 I2C Address: 0x76

  char S1T_colon[6] = "S1-T:"; char *S1T_colonPtr = S1T_colon;
  char S2T_colon[6] = "S2-T:"; char *S2T_colonPtr = S2T_colon;
  char perRH[2] = "%"; char *perRHPtr = perRH;
  char F_RH[6] = "F RH:"; char *F_RH_Ptr = F_RH;
  char degF[2] = {223}; char *degFPtr = degF;
  char tempBME1Array[6]; char *tempBME1ArrayPtr = tempBME1Array;
  char humidBME1Array[6]; char *humidBME1ArrayPtr = humidBME1Array;
  char baroPressBME1Array[6]; char *baroPressBME1ArrayPtr = baroPressBME1Array;

  int counter = 0;
  int avgCounterBME = 1;
  
  float avgTempBME1 = 0.0; 
  float avgHumidBME1 = 0.0; 
  float avgBpBME1 = 0.0;
  float temperatureBME1 = 0.0 ;
  float humidityBME1 = 0.0;  
  float tempValuesBME1 = 0.0; 
  float humidValuesBME1 = 0.0; 
  float baropressBME1 = 0.0;
  float bpValuesBME1 = 0.0;

    char row1[20];  char *row1Ptr = row1; 
  
// LCD Row 2: BME2 I2C Address: 0x77

  float avgTempBME2 = 0.0;
  float avgHumidBME2 = 0.0;
  float temperatureBME2 = 0.0;
  float humidityBME2 = 0.0; 
  float tempValuesBME2 = 0.0;
  float humidValuesBME2 = 0.0;

  
  char tempBME2Array[6]; char *tempBME2ArrayPtr = tempBME2Array;
  char humidBME2Array[6]; char *humidBME2ArrayPtr = humidBME2Array;

  char row2[20]; char *row2Ptr = row2;
 
  // LCD Row 3: Barometric Pressure

  char row3[210]; char *row3Ptr = row3;
  
  uint16_t row3_stringStart, row3_stringStop = 0; //Initialize Row 3 Scrolling Letter Counter
  int row3_scrollCursor = screenWidth;  // LCD Row 3
  
  uint16_t old_row3StrLen = 0;
  uint16_t new_row3StrLen = 0;
  
//******************Timers*********************

//Timers are used to move the character arrays on LCD lines 0,
//to read the BME280's and to get the DarkSky updates. The timing of the
//ThingSpeak update happens after every 20th BME280 read. The DarkSky update
//occurs after every 5th Thingspeak update. See - takeReadingBME280

unsigned long TR_previousMillis = 0; //Take Reading BME280's Timer
unsigned long TR_interval = 3000; //Take Reading BME280's Timer

unsigned long row0_previousMillis = 0; //LCD Row 0 Move Letters
unsigned long row0_interval = 420; //LCD Row 0 Move Letters
unsigned long row0_currentMillis = 0;


//******************Timers*********************

//**************************************************************
//**************** Function Prototypes **************************
//**************************************************************

// Boot and initialization functions
void bme280RecorderBoot();

// WiFi functions
void checkWiFi();

// Time functions
void getNTC_Time(char *dateArrPtr, char *numericalDatePtr, char *timeFPtr);

// Sensor reading functions
bool validateSensorReading(float temp, float humidity, float pressure, const char* sensorName);
void takeReadingBME280(char *row0Ptr, char *numericalDatePtr, char *timeFPtr);

// LCD display functions
void moveLetters(char *row0Ptr);
void printLCD_Row(uint8_t lcdRow, char *rowBuffer, const char *label,
                  const char *tempArray, const char *humidArray);
void printLCD_Row_1();
void printLCD_Row_2();

// Data upload functions
void writeThingSpeak();
void tokenStatusCallback(TokenInfo info);

//**************************************************************
//**********************VOID SETUP******************************
//**************************************************************
void setup() {

Wire.begin();  //Start I2C - I2C WIRE default pins SCL - 22, SDA - 21 - Writing to LCD
Wire1.begin(SDA_2, SCL_2, 100000); // Start 2nd I2C SDA_2 - 33, SCL_2 - 32 - Reading BME-280's

Serial.begin(230400);  //Fast to stop it holding up the stream

// Increment boot counter and display it
bootCount++;
Serial.println();
Serial.println("###################################################");
Serial.print("*** [DIAGNOSTIC] BOOT #");
Serial.print(bootCount);
Serial.println(" ***");
Serial.println("###################################################");
Serial.println();

// Get reset reason
esp_reset_reason_t reset_reason = esp_reset_reason();
Serial.print("Reset reason: ");
switch(reset_reason) {
  case ESP_RST_POWERON:   Serial.println("Power-on reset"); break;
  case ESP_RST_SW:        Serial.println("Software reset via esp_restart()"); break;
  case ESP_RST_PANIC:     Serial.println("Software reset due to exception/panic"); break;
  case ESP_RST_INT_WDT:   Serial.println("Interrupt watchdog reset"); break;
  case ESP_RST_TASK_WDT:  Serial.println("Task watchdog reset"); break;
  case ESP_RST_WDT:       Serial.println("Other watchdog reset"); break;
  case ESP_RST_DEEPSLEEP: Serial.println("Reset after exiting deep sleep mode"); break;
  case ESP_RST_BROWNOUT:  Serial.println("Brownout reset (software or hardware)"); break;
  case ESP_RST_SDIO:      Serial.println("Reset over SDIO"); break;
  default:                Serial.println("Unknown reset reason"); break;
}
Serial.println();

//*******************Watchdog Timer Setup**********************************

// Initialize watchdog timer for system stability
// First, deinitialize any existing watchdog timer to avoid "already initialized" error
esp_task_wdt_deinit();
Serial.println(F("Deinitialized existing watchdog timer"));

// Now initialize with our settings
// API differs between ESP32 Arduino Core versions
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
    // ESP32 Arduino Core 3.0.0+ uses a config struct
    esp_task_wdt_config_t wdt_config = {
        .timeout_ms = WDT_TIMEOUT * 1000,  // Convert seconds to milliseconds
        .trigger_panic = true              // Enable panic so ESP32 restarts on watchdog timeout
    };
    esp_task_wdt_init(&wdt_config);
#else
    // ESP32 Arduino Core 2.x uses function parameters directly
    esp_task_wdt_init(WDT_TIMEOUT, true);  // timeout in seconds, panic enabled
#endif
esp_task_wdt_add(NULL); // Add current thread to watchdog
Serial.println(F("Watchdog timer initialized successfully"));

//*******************LCD Set-up**********************************

int status;
  // see: https://github.com/duinoWitchery/hd44780
  // initialize LCD with number of columns and rows: 
  // hd44780 returns a status from begin() that can be used
  // to determine if initalization failed.
  // the actual status codes are defined in <hd44780.h>
  
  status = lcd.begin(LCD_COLS, LCD_ROWS);
  
  if(status) // non zero status means it was unsuccesful
  {
    status = -status; // convert negative status value to positive number

    // begin() failed so blink error code using the onboard LED if possible
    hd44780::fatalError(status); // does not return
  }

  // turn on automatic line wrapping
  // which automatically wraps lines to the next lower line and wraps back
  // to the top when at the bottom line
  // NOTE: 
  // noLineWrap() can be used to disable automatic line wrapping.
  // _write() can be called instead of write() to send data bytes
  // to the display bypassing any special character or line wrap processing.
  lcd.lineWrap();
//*******************LCD Set-up**********************************


//*******************Set up WiFi connection**********************

 WiFi.mode(WIFI_STA); //This line hides the viewing of ESP as wifi hotspot

 WiFi.begin(ssid, password);

 WiFi.setAutoReconnect(true);
 WiFi.persistent(true);

//*******************Start NTC Clock**********************

timeClient.begin();

//***********************Start ThingSpeak****************************

ThingSpeak.begin(client);  //Thingspeak  

//**************************Start Google Sheets***********************

Serial.println("*** [DIAGNOSTIC] Initializing Google Sheets ***");

// Set the callback for Google API access token generation status (for debug only)
GSheet.setTokenCallback(tokenStatusCallback);

// Set the seconds to refresh the auth token before expire (60 to 3540, default is 300 seconds)
GSheet.setPrerefreshSeconds(10 * 60);

Serial.println("*** [DIAGNOSTIC] Starting Google Sheets token generation ***");
Serial.println("Note: Token generation happens asynchronously in the background");

// Reset watchdog before potentially long operation
esp_task_wdt_reset();

// Begin the access token generation for Google API authentication
// This is non-blocking and will initialize in the background
GSheet.begin(CLIENT_EMAIL, PROJECT_ID, PRIVATE_KEY);

Serial.println("*** [DIAGNOSTIC] Google Sheets begin() called ***");
Serial.println("Token will initialize asynchronously during operation");

// Reset watchdog after operation
esp_task_wdt_reset();


//***************Run Function BME280 RecorderBoot********************    
 /*
  * check BME 280's, check wifi connection, get the time 
  * initialize the LCD, run first DarkSky
  * 
  */
  bme280RecorderBoot();
  
   
  }  //End void setup()

//**************************************************************
//**********************END VOID SETUP**************************
//**************************************************************


//**************************************************************
//************************VOID LOOP*****************************
//**************************************************************

  void loop() {

  // Reset watchdog timer to prevent system restart
  esp_task_wdt_reset();

  // Check WiFi connection status in main loop
  static unsigned long lastWiFiCheck = 0;
  if (millis() - lastWiFiCheck > 60000) { // Check every 60 seconds
    lastWiFiCheck = millis();
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("*** [DIAGNOSTIC] WiFi disconnected in main loop, attempting reconnect ***");
      checkWiFi();
    }
  }

  takeReadingBME280(row0, numericalDate, timeF); //Read and Average BME280 measurements

  moveLetters(row0);  //LCD Row 0


  }

//**************************************************************
//************************END VOID LOOP*************************
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
