# ESP32 BME280 Temperature & Humidity Recorder

Environmental monitoring system using ESP32 with dual BME280 sensors, 20x4 LCD display, and data logging to ThingSpeak and Google Sheets.

## Features

- **Dual BME280 Sensors**: Monitor temperature, humidity, and barometric pressure from two independent sensors
- **LCD Display**: 20x4 character display with scrolling date/time and real-time sensor readings
- **Cloud Logging**: Automatic uploads to both ThingSpeak and Google Sheets
- **NTP Time Sync**: Automatic time synchronization from NTP servers
- **Watchdog Protection**: Automatic recovery from system hangs
- **Sensor Validation**: Validates readings to prevent logging invalid data
- **WiFi Auto-Reconnect**: Automatic WiFi reconnection with timeout protection

## Hardware Requirements

### Components
- ESP32 development board (e.g., ESP32-DevKitC, NodeMCU-32S, LOLIN D32)
- 2x Bosch BME280 sensors (temperature, humidity, barometric pressure)
- 20x4 I2C LCD display (HD44780 compatible)
- Jumper wires
- Power supply (5V via USB or appropriate voltage regulator)

### Wiring Connections

#### I2C Bus 1 (Default Pins - LCD Display)
```
ESP32 Pin   →   LCD Module
GPIO 21     →   SDA
GPIO 22     →   SCL
5V          →   VCC
GND         →   GND
```

#### I2C Bus 2 (Custom Pins - BME280 Sensors)
```
ESP32 Pin   →   BME280 Sensors
GPIO 33     →   SDA (both sensors)
GPIO 32     →   SCL (both sensors)
3.3V        →   VCC (both sensors)
GND         →   GND (both sensors)
```

#### BME280 I2C Address Configuration
- **BME280 #1 (0x76)**: Connect CSB to V+, SDO to GND
- **BME280 #2 (0x77)**: Connect CSB to V+, SDO to V+

> **Note**: The BME280 sensors support two I2C addresses (0x76 and 0x77) selectable via the SDO pin. This allows two sensors on the same bus.

## Software Requirements

### Arduino IDE Setup

1. **Install Arduino IDE** (version 1.8.x or 2.x)
   - Download from: https://www.arduino.cc/en/software

2. **Install ESP32 Board Support**
   - In Arduino IDE, go to File → Preferences
   - Add to "Additional Board Manager URLs":
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - Go to Tools → Board → Boards Manager
   - Search for "ESP32" and install "esp32 by Espressif Systems"

3. **Install Required Libraries**

   Via Arduino Library Manager (Tools → Manage Libraries):

   - **Adafruit BME280 Library** by Adafruit
   - **Adafruit Unified Sensor** by Adafruit
   - **ThingSpeak** by MathWorks
   - **NTPClient** by Fabrice Weinberg
   - **Time** by Michael Margolis
   - **Timezone** by Jack Christensen
   - **hd44780** by Bill Perry
   - **ESP_Google_Sheet_Client** by Mobizt

4. **Modify DateStrings.cpp** (Required for compilation)
   - Navigate to: `Arduino/libraries/Time/DateStrings.cpp`
   - Uncomment line 22: `#include <string.h>`

## Configuration

### 1. Create secrets.h

Copy the template file and add your credentials:

```bash
cp secrets.h.template secrets.h
```

Edit `secrets.h` with your actual values:

```cpp
// WiFi Configuration
#define SECRET_ssid "YourWiFiSSID"
#define SECRET_password "YourWiFiPassword"

// ThingSpeak Configuration
#define SECRET_myChannelNumber 123456
#define SECRET_myWriteAPIKey "YOUR_WRITE_API_KEY"
#define SECRET_api_key "YOUR_API_KEY"

// Sensor Calibration (optional)
#define SECRET_tempCorr 1.0    // Temperature correction factor
#define SECRET_rhCorr 1.0      // Humidity correction factor

// Google Sheets Configuration
#define SECRET_PROJECT_ID "your-project-id"
#define SECRET_CLIENT_EMAIL "your-service-account@project.iam.gserviceaccount.com"
#define SECRET_spreadsheetId "your-spreadsheet-id"
#define SECRET_PRIVATE_KEY PROGMEM "-----BEGIN PRIVATE KEY-----\n...\n-----END PRIVATE KEY-----\n"
```

### 2. ThingSpeak Setup

1. Create a free account at https://thingspeak.com
2. Create a new channel with fields:
   - Field 1: Time
   - Field 2: Date
   - Field 3: BME1 Temperature (°F)
   - Field 4: BME1 Humidity (%)
   - Field 5: BME2 Temperature (°F)
   - Field 6: BME2 Humidity (%)
   - Field 7: Barometric Pressure (inHg)
3. Copy the Channel ID and Write API Key to `secrets.h`

### 3. Google Sheets Setup

1. Create a Google Cloud Project at https://console.cloud.google.com
2. Enable the Google Sheets API
3. Create a Service Account
4. Download the JSON key file
5. Extract the following from the JSON:
   - `project_id` → `SECRET_PROJECT_ID`
   - `client_email` → `SECRET_CLIENT_EMAIL`
   - `private_key` → `SECRET_PRIVATE_KEY`
6. Create a Google Sheet and share it with the service account email
7. Copy the spreadsheet ID from the URL to `SECRET_spreadsheetId`

### 4. Optional: Customize config.h

Modify `config.h` to change default settings:

- Sensor read intervals
- LCD scroll speed
- WiFi timeout
- Serial baud rate
- Validation ranges
- And more...

## Upload and Run

1. Connect your ESP32 to your computer via USB
2. In Arduino IDE:
   - Select your board: Tools → Board → ESP32 → [Your ESP32 Board]
   - Select the port: Tools → Port → [Your COM/USB Port]
3. Click Upload (→ button)
4. Open Serial Monitor (Tools → Serial Monitor) at 230400 baud to see debug output

## Display Information

The 20x4 LCD shows:

```
Row 0: [Scrolling] Date and Time
Row 1: S1-T:XX.X°F RH:XX.X%
Row 2: S2-T:XX.X°F RH:XX.X%
Row 3: BP=XX.X in Hg.
```

## Data Logging Schedule

- **Sensor Readings**: Every 3 seconds
- **LCD Update**: Real-time display of current readings
- **Averaging**: 20 readings (60 seconds)
- **Upload to Cloud**: Every 60 seconds (averaged values)

## Troubleshooting

### BME280 Sensor Not Found
- **Problem**: Serial monitor shows "Could not find BME280 sensor"
- **Solutions**:
  - Check wiring connections
  - Verify I2C addresses (0x76 and 0x77)
  - Ensure SDO pin is configured correctly for each sensor
  - Try swapping sensors to isolate hardware issues

### WiFi Connection Failed
- **Problem**: "WiFi Timeout!" on LCD
- **Solutions**:
  - Verify SSID and password in `secrets.h`
  - Check WiFi signal strength
  - Ensure WiFi network is 2.4GHz (ESP32 doesn't support 5GHz)
  - Increase `WIFI_TIMEOUT_MS` in `config.h`

### LCD Not Displaying
- **Problem**: Blank LCD screen
- **Solutions**:
  - Check I2C connections (SDA/SCL)
  - Adjust LCD contrast potentiometer
  - Verify LCD I2C address (usually 0x27 or 0x3F)
  - Check power supply (5V for LCD)

### ThingSpeak Update Failures
- **Problem**: "Problem updating channel" in serial monitor
- **Solutions**:
  - Verify Write API Key in `secrets.h`
  - Check internet connectivity
  - ThingSpeak free accounts limited to 15-second intervals (code uses 60s)
  - Review HTTP error codes in serial monitor

### Google Sheets Update Failures
- **Problem**: "Google Sheets update UNSUCCESSFUL"
- **Solutions**:
  - Verify service account credentials
  - Check spreadsheet sharing permissions
  - Ensure Google Sheets API is enabled
  - Verify spreadsheet ID is correct

### Invalid Sensor Readings
- **Problem**: "ERROR: Temperature out of range" or NaN readings
- **Solutions**:
  - Check sensor connections
  - Verify sensor power supply (3.3V)
  - Adjust validation ranges in `config.h` if needed
  - Replace faulty sensor

### System Hangs or Freezes
- **Problem**: ESP32 stops responding
- **Solutions**:
  - Watchdog timer should auto-restart after 30 seconds
  - Check for infinite loops in custom code
  - Increase `WDT_TIMEOUT` in main .ino file if needed
  - Monitor heap memory usage in serial monitor

## Code Structure

```
TRh_BME280_Esp32/
├── TRh_BME280_Esp32.ino      # Main program file
├── BME_Recorder_Boot.ino      # Boot sequence and initialization
├── checkWiFi.ino              # WiFi connection management
├── getNTC_Time.ino            # NTP time synchronization
├── takeReadingBME280.ino      # Sensor reading and validation
├── moveLetters_Row_0.ino      # LCD scrolling display
├── writeThingSpeak.ino        # Cloud upload functions
├── config.h                   # Configuration constants
├── secrets.h                  # Credentials (not in git)
├── secrets.h.template         # Template for credentials
├── .gitignore                 # Git ignore file
└── README.md                  # This file
```

## Security Notes

- **NEVER** commit `secrets.h` to version control
- The `.gitignore` file protects sensitive files
- Use `secrets.h.template` as a reference
- Rotate credentials if accidentally exposed
- Consider using environment variables for production deployments

## Customization

### Changing Sensor Read Interval

Edit `config.h`:
```cpp
#define SENSOR_READ_INTERVAL 5000  // 5 seconds instead of 3
```

### Adjusting Calibration

Edit `secrets.h`:
```cpp
#define SECRET_tempCorr 0.984  // Adjust based on calibration
#define SECRET_rhCorr 0.92     // Adjust based on calibration
```

### Changing LCD Scroll Speed

Edit `config.h`:
```cpp
#define LCD_SCROLL_INTERVAL 300  // Faster scrolling (was 420)
```

## License

MIT License - See source files for full license text.

## Credits

- Adafruit for BME280 libraries
- Bill Perry for hd44780 LCD library
- MathWorks for ThingSpeak library
- Mobizt for Google Sheets client
- ESP32 community for board support

## Support

For issues, questions, or contributions:
- File issues on GitHub
- Check serial monitor output for debugging
- Review ThingSpeak and Google Sheets API documentation

## Version History

- **v2.0** - Added watchdog, sensor validation, better error handling, config.h
- **v1.0** - Initial release with dual BME280 support

---

**Happy Monitoring!** 🌡️ 💧 📊
