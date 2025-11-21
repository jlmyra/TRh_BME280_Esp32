//**************************************************************
//***************Function to Write ThingSpeak*******************
//**************************************************************
void writeThingSpeak() {
  
 int x = 0; 
  
  getNTC_Time(dateArr, numericalDate, timeF);
  
  // set the fields with the values
  ThingSpeak.setField(1,timeFPtr); //Time
  ThingSpeak.setField(2,numericalDatePtr); //Date
  ThingSpeak.setField(3,avgTempBME1); //Ambient T
  ThingSpeak.setField(4,avgHumidBME1); //Ambient Rh
  ThingSpeak.setField(5,avgTempBME2); //HC T
  ThingSpeak.setField(6,avgHumidBME2); //HC Rh
  ThingSpeak.setField(7,avgBpBME1); //BP Ambient
  
  // write to the ThingSpeak channel
  //Serial.println("at TS writeFields"); Serial.println(" ");

  checkWiFi();

  x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  FirebaseJson response;
  
  FirebaseJson valueRange;

        valueRange.add("majorDimension", "COLUMNS");
        valueRange.set("values/[0]/[0]", formattedTime);
        valueRange.set("values/[1]/[0]", avgTempBME1);
        valueRange.set("values/[2]/[0]", avgHumidBME1);
        valueRange.set("values/[3]/[0]", avgTempBME2);
        valueRange.set("values/[4]/[0]", avgHumidBME2);
        valueRange.set("values/[5]/[0]", avgBpBME1);

// Check if Google Sheets is ready before attempting to write
Serial.println("*** [DIAGNOSTIC] Checking Google Sheets ready status ***");
esp_task_wdt_reset(); // Reset watchdog before Google Sheets operations

if (GSheet.ready()) {
    Serial.println("*** [DIAGNOSTIC] Google Sheets is READY - attempting to write ***");
    bool success = GSheet.values.append(&response /* returned response */, spreadsheetId /* spreadsheet Id to append */, "Sheet1!A1" /* range to append */, &valueRange /* data range to append */);
    esp_task_wdt_reset(); // Reset watchdog after API call
    if (success){
        Serial.println();
        Serial.println("###################################################");
        Serial.println(".     Google Sheets update successful");
        Serial.println("###################################################");
        Serial.println();
        response.toString(Serial, true);
        valueRange.clear();
        Serial.println();
    }
    else{
        Serial.println();
        Serial.println("###################################################");
        Serial.println(".     Google Sheets update UNSUCCESSFUL");
        Serial.println("###################################################");
        Serial.println();
        Serial.println("Error reason: " + String(GSheet.errorReason()));
        Serial.println("*** [DIAGNOSTIC] Additional error details ***");
        Serial.println("HTTP Code: " + String(GSheet.errorCode()));
        Serial.println("Response object:");
        response.toString(Serial, true);
        Serial.println();
        Serial.println("Token status at time of error:");
        GSheet.printf("Token type: %s\n", GSheet.getTokenType().c_str());
        GSheet.printf("Token status: %s\n", GSheet.getTokenStatus().c_str());
        Serial.println("SpreadsheetId: " + String(spreadsheetId));
        Serial.println("Value range data being sent:");
        valueRange.toString(Serial, true);
        Serial.println();
        Serial.println("WiFi Status: " + String(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected"));
        Serial.println("WiFi RSSI: " + String(WiFi.RSSI()));
        Serial.println("Continuing without restart - will retry on next cycle");
        Serial.println();
    }
} else {
    Serial.println();
    Serial.println("###################################################");
    Serial.println(".     Google Sheets NOT READY - Token initializing");
    Serial.println("###################################################");
    Serial.println("Skipping Google Sheets update this cycle");
    Serial.println("This is normal during startup and will resolve automatically");
    Serial.println();
}
        Serial.println();
        Serial.println(ESP.getFreeHeap());
    



/*
#define TIMEOUT_MS_SERVERRESPONSE 5000  // Wait up to five seconds for server to respond

#define OK_SUCCESS              200     // OK / Success
#define ERR_BADAPIKEY           400     // Incorrect API key (or invalid ThingSpeak server address)
#define ERR_BADURL              404     // Incorrect API key (or invalid ThingSpeak server address)
#define ERR_OUT_OF_RANGE        -101    // Value is out of range or string is too long (> 255 bytes)
#define ERR_INVALID_FIELD_NUM   -201    // Invalid field number specified
#define ERR_SETFIELD_NOT_CALLED -210    // setField() was not called before writeFields()
#define ERR_CONNECT_FAILED      -301    // Failed to connect to ThingSpeak
#define ERR_UNEXPECTED_FAIL     -302    // Unexpected failure during write to ThingSpeak
#define ERR_BAD_RESPONSE        -303    // Unable to parse response
#define ERR_TIMEOUT             -304    // Timeout waiting for server to respond
#define ERR_NOT_INSERTED        -401    // Point was not inserted (most probable cause is the rate limit of once every 15 seconds)
*/ 
 
  if(x == 200 || x == -201 || x == -210){
    Serial.println("###################################################");
    Serial.println("Thingspeak update successful. Error code: " + String (x));
    Serial.println("###################################################");
    Serial.println();
   }
  else
   {
    int y = 0;
      while ((x != 200) && (x != -201) && (x != -210))
      {
        if (y == 5)
        {
         Serial.println();
         Serial.println("############### ThingSpeak write failed 5 times --- RESTARTING ESP32 #################");
         Serial.println();
         
         ESP.restart(); 
        }
      
        lcd.clear();
        lcd.setCursor(0,2); lcd.print("Problem updating TS ");
        lcd.setCursor(0,3); lcd.print("HTTP error: "); lcd.print(String(x));
        //delay(3000);
        Serial.println();
        Serial.println("@@@@@@@@@@@@@@@@ Problem updating channel. HTTP error code: " + String(x) + " @@@@@@@@@@@@@@@@@");
        Serial.println();
        Serial.println("############# Resetting WiFi Connection - void resetWiFi() ##############");
        Serial.println();
    
        WiFi.disconnect(true);
    
        delay(2000); //1000 not long enough
    
        WiFi.begin(ssid, password); 

        checkWiFi();

        x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
        
        if( x == -210){
        Serial.println();
        Serial.println("############# Breaking out of while loop - in writeThingSpeak - HTTP error code: " + String(x) + " ##############");
        Serial.println();
        break;
        }

        y = y + 1;
      }
    }
  Serial.println();
  Serial.print(row0Ptr); Serial.println(" Row 0 Length= " + String(strlen(row0Ptr)));
  Serial.print(row1Ptr); Serial.println(" Row 1 Length= " + String(strlen(row1Ptr)));
  Serial.print(row2Ptr); Serial.println(" Row 2 Length= " + String(strlen(row2Ptr)));
  Serial.print(row3Ptr); Serial.println(" Row 3 Length= " + String(strlen(row3Ptr)));
  Serial.println();

Serial.print("Formatted Time: "); Serial.println(formattedTime); 
Serial.println();
}

void tokenStatusCallback(TokenInfo info){
    Serial.println("*** [DIAGNOSTIC] Google Sheets Token Status Update ***");
    esp_task_wdt_reset(); // Reset watchdog during token operations
    if (info.status == token_status_error){
        Serial.println("!!! TOKEN ERROR DETECTED !!!");
        GSheet.printf("Token info: type = %s, status = %s\n", GSheet.getTokenType(info).c_str(), GSheet.getTokenStatus(info).c_str());
        GSheet.printf("Token error: %s\n", GSheet.getTokenError(info).c_str());
    }
    else{
        GSheet.printf("Token info: type = %s, status = %s\n", GSheet.getTokenType(info).c_str(), GSheet.getTokenStatus(info).c_str());
        if (info.status == token_status_ready) {
            Serial.println("*** Google Sheets token is READY ***");
        }
    }
}
//**************************************************************
//***********END Function to Write ThingSpeak*******************
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
