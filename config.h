// config.h
// Non-secret configuration settings for BME280 Temperature & Humidity Recorder
// For sensitive credentials (WiFi, API keys, etc.), see secrets.h

#ifndef CONFIG_H
#define CONFIG_H

//*********************Hardware Configuration*********************

// I2C Configuration
// I2C WIRE default pins: SCL - 22, SDA - 21 (for LCD)
// I2C WIRE1 custom pins: SDA_2 - 33, SCL_2 - 32 (for BME280s)
#define SDA_2 33
#define SCL_2 32
#define I2C_FREQUENCY 100000  // 100kHz

// BME280 I2C Addresses
#define BME1_I2C_ADDRESS 0x76  // CSB to V+, SDO to GND
#define BME2_I2C_ADDRESS 0x77  // CSB to V+, SDO to V+

// LCD Configuration
#define LCD_COLS 20
#define LCD_ROWS 4

//*********************Timing Configuration*********************

// Sensor Reading Intervals
#define SENSOR_READ_INTERVAL 3000     // 3 seconds between sensor reads
#define AVERAGING_COUNT 20            // Number of readings to average (20 * 3s = 60s)

// LCD Scrolling
#define LCD_SCROLL_INTERVAL 420       // Milliseconds between scroll updates

// WiFi Configuration
#define WIFI_TIMEOUT_MS 30000         // 30 second WiFi connection timeout
#define WIFI_RETRY_DELAY 2000         // 2 second delay between retry attempts

// ThingSpeak Configuration
#define THINGSPEAK_UPDATE_INTERVAL 60000  // 1 minute (SENSOR_READ_INTERVAL * AVERAGING_COUNT)
#define THINGSPEAK_MAX_RETRIES 5          // Maximum retry attempts for ThingSpeak updates

// NTP Time Configuration
#define NTP_OFFSET (60 * 60)          // Time offset in seconds
#define NTP_INTERVAL (60 * 1000)      // Update interval in milliseconds
#define NTP_ADDRESS "north-america.pool.ntp.org"  // NTP server pool

// Year Validation Range (for NTP time validation)
#define MIN_VALID_YEAR 2020
#define MAX_VALID_YEAR 2099

//*********************Serial Communication*********************

#define SERIAL_BAUD_RATE 230400       // Serial communication speed

//*********************Debug Configuration*********************

// Uncomment to enable debug output
// #define DEBUG_MODE true

#ifdef DEBUG_MODE
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif

//*********************Sensor Validation Ranges*********************

// Temperature validation (Fahrenheit)
#define MIN_VALID_TEMP_F -40.0
#define MAX_VALID_TEMP_F 185.0

// Humidity validation (percent)
#define MIN_VALID_HUMIDITY 0.0
#define MAX_VALID_HUMIDITY 100.0

// Barometric pressure validation (inches of Mercury)
#define MIN_VALID_PRESSURE_INHG 20.0
#define MAX_VALID_PRESSURE_INHG 35.0

//*********************Display Configuration*********************

#define SCREEN_WIDTH 20
#define SCREEN_HEIGHT 4

// Buffer sizes for character arrays
#define TIME_BUFFER_SIZE 11
#define DATE_BUFFER_SIZE 24
#define NUMERICAL_DATE_SIZE 12
#define ROW0_BUFFER_SIZE 77
#define ROW_BUFFER_SIZE 20
#define ROW3_BUFFER_SIZE 210

//*********************Unit Conversion Constants*********************

#define CELSIUS_TO_FAHRENHEIT(c) (((c) * 1.8) + 32)
#define PASCAL_TO_INHG 0.000295333727

#endif // CONFIG_H
