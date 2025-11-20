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

  dtostrf(temperatureBME1, 3, 1, tempBME1ArrayPtr); //convert floats to arrays
  dtostrf(humidityBME1, 3, 1, humidBME1ArrayPtr);
  dtostrf(baropressBME1, 3, 1, baroPressBME1ArrayPtr);

  printLCD_Row_1(); // Function to print row 1
  Serial.println();
   Serial.print("row1Ptr length (average) = "); Serial.println(strlen (row1Ptr)); 

  lcd.setCursor(0, 3);   //
  lcd.print("BP= "); //LCD print the Array
  lcd.setCursor(4, 3);
  lcd.print(baropressBME1);
  lcd.setCursor(9, 3);
  lcd.print("in Hg.   ");

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

  dtostrf(temperatureBME2, 3, 1, tempBME2ArrayPtr); //convert floats to arrays
  dtostrf(humidityBME2, 3, 1, humidBME2ArrayPtr);
      
  printLCD_Row_2(); // Function to print row 2
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
  dtostrf(temperatureBME1, 3, 1, tempBME1ArrayPtr); //convert floats to arrays
  dtostrf(humidityBME1, 3, 1, humidBME1ArrayPtr);
  baropressBME1 = ((bme1.readPressure()) * 0.000295333727 );
  delayMicroseconds(50);

  printLCD_Row_1();  //Function to LCD print row 1

  Serial.print("Average Counter= ");Serial.print (avgCounterBME); Serial.print("  row1Ptr length (real time) = "); Serial.println(strlen (row1Ptr));
  
  //####################### LCD Print Row 3 ###########################

  lcd.setCursor(0, 3);   //Display readings on LCD 
  lcd.print("BP= "); //LCD print the Array
  lcd.setCursor(4, 3);
  lcd.print(baropressBME1);
  lcd.setCursor(9, 3);
  lcd.print("in Hg.   ");
  
  
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
  dtostrf(temperatureBME2, 3, 1, tempBME2ArrayPtr); //convert floats to arrays
  dtostrf(humidityBME2, 3, 1, humidBME2ArrayPtr);
  
  
  printLCD_Row_2();
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
//** Function to display ROW 1 on LCD**
//**************************************************************

void printLCD_Row_1(){

    for (unsigned int i = 0, j = 0; i < 5 && j < 5; i++, j++){  //Put "T:" into Array 
    row1Ptr[i] = S1T_colonPtr[j];
    }
    for (unsigned int i = 5, j = 0; i < (strlen(tempBME1ArrayPtr) + 5) && j < strlen(tempBME1ArrayPtr); i++, j++){ // Put Temperature into Array
    row1Ptr[i] = tempBME1ArrayPtr[j]; 
    }
    row1Ptr[strlen(tempBME1ArrayPtr) + 5] = {223}; //Put degree symbol into Array
    for (unsigned int i = (strlen(tempBME1ArrayPtr) + 6), j = 0; i < (strlen(tempBME1ArrayPtr) + 11) && j < 5; i++, j++){ //Put "F RH:" into Array
    row1Ptr[i] = F_RH_Ptr[j]; 
    }    
    for (unsigned int i = (strlen(tempBME1ArrayPtr) + 11), j = 0; i < ((strlen(tempBME1ArrayPtr) + 11) + (strlen(humidBME1ArrayPtr))) && j < strlen(humidBME1ArrayPtr); i++, j++){
    row1Ptr[i] = humidBME1ArrayPtr[j];  //Put RH in to the Array
    }
    row1Ptr[((strlen(tempBME1ArrayPtr) + 11) + (strlen(humidBME1ArrayPtr)))] = perRHPtr[0]; //Put "%" into the array
    
    row1Ptr[19] = 0;

  //if(strlen((tempBME1ArrayPtr) + strlen(humidBME1ArrayPtr)) <= 11){ // End the array with null characters to clean the row after 5 or 6 digit TEMP or RH reading
    //row1Ptr[17] = ' ';
    //row1Ptr[18] = ' ';
    //row1Ptr[19] = 0;
    //}
  
  lcd.setCursor(0, 1);   //Display readings on LCD (position 0, row 1 or line 2)
  lcd.print(row1); //LCD print the Array
}
//**************************************************************
//** END Function to display ROW 1 on LCD**
//**************************************************************


//**************************************************************
//** Function to display ROW 2 on LCD**
//**************************************************************

void printLCD_Row_2(){//was row1

    for (unsigned int i = 0, j = 0; i < 5 && j < 5; i++, j++){  //Put "T:" into Array 
    row2Ptr[i] = S2T_colonPtr[j];
    }
    for (unsigned int i = 5, j = 0; i < (strlen(tempBME2ArrayPtr) + 5) && j < strlen(tempBME2ArrayPtr); i++, j++){ // Put Temperature into Array
    row2Ptr[i] = tempBME2ArrayPtr[j]; 
    }
    row2Ptr[strlen(tempBME2ArrayPtr) + 5] = {223}; //Put degree symbol into Array
    for (unsigned int i = (strlen(tempBME2ArrayPtr) + 6), j = 0; i < (strlen(tempBME2ArrayPtr) + 11) && j < 5; i++, j++){ //Put "F RH:" into Array
    row2Ptr[i] = F_RH_Ptr[j]; 
    }    
    for (unsigned int i = (strlen(tempBME2ArrayPtr) + 11), j = 0; i < ((strlen(tempBME2ArrayPtr) + 11) + (strlen(humidBME2ArrayPtr))) && j < strlen(humidBME2ArrayPtr); i++, j++){
    row2Ptr[i] = humidBME2ArrayPtr[j];  //Put RH in to the Array
    }
    row2Ptr[((strlen(tempBME2ArrayPtr) + 11) + (strlen(humidBME2ArrayPtr)))] = perRHPtr[0]; //Put "%" into the array
    
    row2Ptr[19] = 0;
    
    /*
  if(strlen((tempBME2ArrayPtr) + strlen(humidBME2ArrayPtr)) <= 8){ // End the array with null characters to clean the row after 5 or 6 digit TEMP or RH reading
    row2Ptr[12] = ' ';
    row2Ptr[13] = ' ';
    row2Ptr[14] = 0;
    }
  */
  lcd.setCursor(0, 2);   //Display readings on LCD (position 0, row 1 or line 2)
  lcd.print(row2); //LCD print the Array
  
}
//**************************************************************
//** END Function to display ROW 1 on LCD**
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
