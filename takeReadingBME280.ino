//**************************************************************
//****Helper Function to Validate Sensor Readings****
//**************************************************************

bool validateSensorReading(float temp, float humidity, float pressure, const char* sensorName) {
  // Check for NaN values
  if (isnan(temp) || isnan(humidity) || isnan(pressure)) {
    Serial.print("ERROR: NaN reading from ");
    Serial.println(sensorName);
    return false;
  }

  // Validate temperature range (Fahrenheit): -40°F to 185°F (BME280 spec)
  if (temp < -40.0 || temp > 185.0) {
    Serial.print("ERROR: Temperature out of range from ");
    Serial.print(sensorName);
    Serial.print(": ");
    Serial.println(temp);
    return false;
  }

  // Validate humidity range: 0% to 100%
  if (humidity < 0.0 || humidity > 100.0) {
    Serial.print("ERROR: Humidity out of range from ");
    Serial.print(sensorName);
    Serial.print(": ");
    Serial.println(humidity);
    return false;
  }

  // Validate pressure range (inHg): 20 to 35 (reasonable atmospheric range)
  if (pressure < 20.0 || pressure > 35.0) {
    Serial.print("ERROR: Pressure out of range from ");
    Serial.print(sensorName);
    Serial.print(": ");
    Serial.println(pressure);
    return false;
  }

  return true;
}

//**************************************************************
//****Function to Read the BME1&2 Sensors and report to LCD****
//**************************************************************

  void takeReadingBME280(char *row0Ptr, char *numericalDatePtr , char *timeFPtr){
    
  unsigned long TR_currentMillis = millis();
  if (TR_currentMillis - TR_previousMillis > TR_interval) {
  TR_previousMillis = TR_currentMillis;  
  
  if (avgCounterBME == 20) {
    

  //*****************Read BME280 #1, Average,  display on LCD row 1********************

  humidityBME1 = (bme1.readHumidity() * rhCorr);

  delayMicroseconds(50);

  temperatureBME1 = (((bme1.readTemperature() * 1.8) + 32) * tempCorr);
  Serial.println();
  Serial.print("Average temperature BME1: = "); Serial.println (temperatureBME1);
  Serial.print("Average temperature BME2: = "); Serial.println (temperatureBME2);
  Serial.println();

  delayMicroseconds(50);
  baropressBME1 = ((bme1.readPressure()) * 0.000295333727 );
  delayMicroseconds(50);

  // Validate BME1 readings
  if (!validateSensorReading(temperatureBME1, humidityBME1, baropressBME1, "BME1")) {
    Serial.println("WARNING: Invalid reading from BME1, skipping this measurement");
    return; // Skip this reading cycle
  }  

  dtostrf(temperatureBME1, 3, 1, tempBME1ArrayPtr); //convert floats to arrays
  dtostrf(humidityBME1, 3, 1, humidBME1ArrayPtr);
  dtostrf(baropressBME1, 3, 1, baroPressBME1ArrayPtr);

  printTFT_Row_1(); // Function to print row 1 on TFT
  Serial.println();
   Serial.print("row1Ptr length (average) = "); Serial.println(strlen (row1Ptr));

  // Display barometric pressure on row 3 (y = 180)
  tft.setCursor(10, 180);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextSize(2);
  tft.print("BP= ");
  tft.print(baropressBME1);
  tft.print(" inHg   ");

     humidValuesBME1=humidValuesBME1 + humidityBME1;  // Sum accumulation for average calculation
     tempValuesBME1=tempValuesBME1 + temperatureBME1; // Sum accumulation for average calculation
     bpValuesBME1 = bpValuesBME1 + baropressBME1;

     avgHumidBME1 = humidValuesBME1 / (avgCounterBME); // Compute Average Humidity
     avgTempBME1 = tempValuesBME1 / (avgCounterBME);   // Compute Average Temperature
     avgBpBME1 = bpValuesBME1 / (avgCounterBME);

  //**************************BME2 - (0x77)********************************************
  //*****************Read BME280 #2, Average,  display on LCD row 1********************

  humidityBME2 = (bme2.readHumidity() * rhCorr) ;

  delayMicroseconds(50);
  temperatureBME2 = (((bme2.readTemperature() * 1.8) + 32) * tempCorr);
  delayMicroseconds(50);

  // Validate BME2 readings (pressure not read for BME2, use 30.0 as placeholder)
  if (!validateSensorReading(temperatureBME2, humidityBME2, 30.0, "BME2")) {
    Serial.println("WARNING: Invalid reading from BME2, skipping this measurement");
    return; // Skip this reading cycle
  }   

  dtostrf(temperatureBME2, 3, 1, tempBME2ArrayPtr); //convert floats to arrays
  dtostrf(humidityBME2, 3, 1, humidBME2ArrayPtr);

  printTFT_Row_2(); // Function to print row 2 on TFT
  Serial.print("row2Ptr length (average) = "); Serial.println(strlen (row2Ptr));
  Serial.println();

     humidValuesBME2=humidValuesBME2 + humidityBME2;  // Sum accumulation for average calculation
     tempValuesBME2=tempValuesBME2 + temperatureBME2; // Sum accumulation for average calculation
    
     avgHumidBME2 = humidValuesBME2 / (avgCounterBME); // Compute Average Humidity
     avgTempBME2 = tempValuesBME2 / (avgCounterBME);   // Compute Average Temperature
     
//***************************************************************************
//******************** Write Averages to THINGSPEAK ***********************
//***************************************************************************
     
     writeThingSpeak(); //happens every 1 minutes 3000ms x 20 readings = 60000ms

     tempValuesBME1 = 0.0;  //Zero out accumulator for next average
     humidValuesBME1 = 0.0;  //Zero out accumulator for next average
     bpValuesBME1 = 0.0;
     
     tempValuesBME2 = 0.0;  //Zero out accumulator for next average
     humidValuesBME2 = 0.0;  //Zero out accumulator for next average

     avgCounterBME = 1;    //Zero out accumulator for next average
 }
//***************************************************************************
//****************** END Write Averages to THINGSPEAK ***********************
//***************************************************************************

  else          //avgCounter < 20 CONTINUE ACCUMULATING DATA UNTIL 20 READINGS REACHED
    {
      
//***************************************************************************
//******************** Build Data Array for LCD Row 0 ***********************
//***************************************************************************
  old_row0StrLen = strlen(row0Ptr);
  row0Ptr[0] = 0;
 
uint16_t dateTimeLen = strlen(timeFPtr) + strlen(numericalDatePtr);

for (unsigned int i = 0; i < strlen(numericalDatePtr); i++){
  row0Ptr[i] = numericalDatePtr[i];
}
for (unsigned int i = strlen(numericalDatePtr), j = 0; i < dateTimeLen && j < strlen(timeFPtr); i++, j++){
  row0Ptr[i] = timeFPtr[j]; 

row0Ptr[dateTimeLen + 1] = ' ';
row0Ptr[dateTimeLen + 2] = 0;

new_row0StrLen = strlen(row0Ptr);

}

//***************************************************************************
//*****************END Build Data Array for LCD Row 0 ***********************
//***************************************************************************


//**************************BME1 - (0x767)********************************************
  //*****************Read BME280 #1 and display on LCD row 1***************************

  humidityBME1 = bme1.readHumidity() * rhCorr;
  delayMicroseconds(50);
  temperatureBME1 = (((bme1.readTemperature() * 1.8) + 32) * tempCorr);
  delayMicroseconds(50);
  baropressBME1 = ((bme1.readPressure()) * 0.000295333727 );
  delayMicroseconds(50);

  // Validate BME1 real-time readings
  if (!validateSensorReading(temperatureBME1, humidityBME1, baropressBME1, "BME1")) {
    Serial.println("WARNING: Invalid real-time reading from BME1, skipping this measurement");
    return; // Skip this reading cycle
  }

  dtostrf(temperatureBME1, 3, 1, tempBME1ArrayPtr); //convert floats to arrays
  dtostrf(humidityBME1, 3, 1, humidBME1ArrayPtr);

  printTFT_Row_1();  //Function to TFT print row 1

  Serial.print("Average Counter= ");Serial.print (avgCounterBME); Serial.print("  row1Ptr length (real time) = "); Serial.println(strlen (row1Ptr));

  //####################### TFT Print Row 3 ###########################

  // Display barometric pressure on row 3 (y = 180)
  tft.setCursor(10, 180);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextSize(2);
  tft.print("BP= ");
  tft.print(baropressBME1);
  tft.print(" inHg   ");
  
  
     humidValuesBME1 = humidValuesBME1 + humidityBME1;  // Sum accumulation for average calculation
     tempValuesBME1 = tempValuesBME1 + temperatureBME1; // Sum accumulation for average calculation
     bpValuesBME1 = bpValuesBME1 + baropressBME1;
     //avgCounterBME = avgCounterBME + 1; //Increment the Average Counter
     

//**************************************************************
//**END Function to Read the BME1 Sensor and report to LCD**
//**************************************************************

//**************************BME2 - (0x77)********************************************
//*****************Read BME280 #2 and display on LCD row 2***************************

  humidityBME2 = bme2.readHumidity() * rhCorr;
  delayMicroseconds(50);
  temperatureBME2 = (((bme2.readTemperature() * 1.8) + 32) * tempCorr);
  delayMicroseconds(50);

  // Validate BME2 real-time readings (pressure not read for BME2, use 30.0 as placeholder)
  if (!validateSensorReading(temperatureBME2, humidityBME2, 30.0, "BME2")) {
    Serial.println("WARNING: Invalid real-time reading from BME2, skipping this measurement");
    return; // Skip this reading cycle
  }

  dtostrf(temperatureBME2, 3, 1, tempBME2ArrayPtr); //convert floats to arrays
  dtostrf(humidityBME2, 3, 1, humidBME2ArrayPtr);


  printTFT_Row_2();
  Serial.print("Average Counter= "); Serial.print (avgCounterBME);Serial.print("  row2Ptr length (real time) = "); Serial.println(strlen (row2Ptr));
  
     humidValuesBME2 = humidValuesBME2 + humidityBME2;  // Sum accumulation for average calculation
     tempValuesBME2 = tempValuesBME2 + temperatureBME2; // Sum accumulation for average calculation
   
     avgCounterBME = avgCounterBME + 1; //Increment the Average Counter
     }
   
  }
 }
//**************************************************************
//**END Function to Read the BME1&2 Sensor and report to LCD**
//**************************************************************



//**************************************************************
//** Consolidated Function to display sensor data on TFT**
//**************************************************************

void printTFT_Row(uint8_t tftRow, char *rowBuffer, const char *label,
                  const char *tempArray, const char *humidArray, uint16_t color) {

    // Build the display string: "Sx-T:XX.XF RH:XX.X%"
    // Copy sensor label (e.g., "S1-T:")
    for (unsigned int i = 0, j = 0; i < 5 && j < 5; i++, j++){
        rowBuffer[i] = label[j];
    }

    // Copy temperature value
    for (unsigned int i = 5, j = 0; i < (strlen(tempArray) + 5) && j < strlen(tempArray); i++, j++){
        rowBuffer[i] = tempArray[j];
    }

    // Add degree symbol (use 'F' for simplicity on TFT)
    rowBuffer[strlen(tempArray) + 5] = 'F';

    // Add " RH:" label (simplified spacing)
    rowBuffer[strlen(tempArray) + 6] = ' ';
    for (unsigned int i = (strlen(tempArray) + 7), j = 3; i < (strlen(tempArray) + 11) && j < 5; i++, j++){
        rowBuffer[i] = F_RH_Ptr[j];
    }

    // Copy humidity value
    for (unsigned int i = (strlen(tempArray) + 11), j = 0;
         i < ((strlen(tempArray) + 11) + (strlen(humidArray))) && j < strlen(humidArray);
         i++, j++){
        rowBuffer[i] = humidArray[j];
    }

    // Add percent symbol
    rowBuffer[((strlen(tempArray) + 11) + (strlen(humidArray)))] = perRHPtr[0];

    // Null terminate the string
    rowBuffer[((strlen(tempArray) + 11) + (strlen(humidArray))) + 1] = 0;

    // Calculate Y position based on row number
    // Row 1 = y:60, Row 2 = y:120, Row 3 = y:180
    uint16_t yPos = tftRow * 60;

    // Display on TFT at calculated position
    tft.setCursor(10, yPos);
    tft.setTextColor(color, TFT_BLACK);
    tft.setTextSize(2);
    tft.print(rowBuffer);
    tft.print("    "); // Clear any extra characters
}

//**************************************************************
//** Wrapper functions for backward compatibility**
//**************************************************************

void printTFT_Row_1(){
    printTFT_Row(1, row1, S1T_colon, tempBME1Array, humidBME1Array, TFT_CYAN);
}

void printTFT_Row_2(){
    printTFT_Row(2, row2, S2T_colon, tempBME2Array, humidBME2Array, TFT_GREEN);
}

//**************************************************************
//** END Consolidated TFT display functions**
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
