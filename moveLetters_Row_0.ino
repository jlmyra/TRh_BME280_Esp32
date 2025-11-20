//**************************************************************
//***********Function to Scroll Row 0 on LCD*******************
//**************************************************************
// This function scrolls the letters of a line from right to left. It was inspired by "Single line scroll
// from Nishant Arora-https://goo.gl/qPGpWB"
// It first tests to see if the line length has changed. If the length of the printed array has changed
// the counters have to be reset to accomodate new line length.
// On each pass through the function a segment of the array is displayed. This segment is created by 
// assessing the conditions below. The test is ignored until the 2nd DarkSky update (&& GDSCounter > 1).
// 1. The first 20 characters of the line (see First "if" below) The cursor starts at 20 and moves to  
//    0 with each pass. It creates a group of characters that increases by one with each pass
// 2. The next groups of 20 characters (see Fourth "if' below) When the cursor reaches 0 then it 
//    displays a group of characters that takes one off the beginning and adds one
//    to the end until it reaches less than 20 characters left
// 3. The end of the line that is less than 20 characters (see Third 'if' below)
// 4. The last of the 20 characters and spaces have been displayed reset counters and start over
//    (see Second 'if' below)


void moveLetters(char *row0Ptr){

  row0_currentMillis = millis();
  if (row0_currentMillis - row0_previousMillis > row0_interval) {
      row0_previousMillis = row0_currentMillis; 
      
  //Test to see if Line length has changed    
   
    if(old_row0StrLen != new_row0StrLen)
    {

    stringStart = stringStop = 0;
    scrollCursor = screenWidth;

   Serial.println();
   Serial.println(F("                ##########################################################"));   
   Serial.println(F("                @@@@@@@@@@@@ mlr0 Resetting start, stop & width @@@@@@@@@@"));
   Serial.println(F("                ##########################################################"));
   Serial.println();
   Serial.println(F("Function row0 after strlen"));
   
   old_row0StrLen = strlen(row0Ptr);
    }
    
//Begin lcd.print Routine

    lcd.setCursor(scrollCursor, 0);//scrollCursor starts at 20 stringStart, stringStart start at 0
          
//***-FIRST-'if' to lcd.print the first 20 characters 
       
      if(stringStart == 0 && scrollCursor > 0) //***-FIRST-'if'
      {              
      for (int i = 0; i < stringStop; i++)
        {
      lcd.print (row0Ptr[i]); //***PRINT TO LCD***
        }     
      scrollCursor--;
      stringStop++;          
      } 

 //***-SECOND-'if' to lcd.print End of line reached--reset everything
 
      else
       
      if (stringStart == stringStop) //***-SECOND-'if'
      {
      stringStart = stringStop = 0;
      scrollCursor = screenWidth;     
      } 

//*--THIRD-- if: Print the last 20 characters of the line

  else
  
  if (stringStop == strlen(row0Ptr) && scrollCursor == 0) //*--THIRD-- if: Print the last 20 characters of the line
  {     
        for (int i = stringStart; i< stringStop; i++)
        {
        lcd.print(row0Ptr[i]);//***PRINT TO LCD***
        }
        
      stringStart++;
  }  
//---Fourth if line0: lcd.print next 20 character chunks shifting the line by one character  
      else 
      {      
        for (int i = stringStart; i< stringStop; i++)
        {      
      lcd.print(row0Ptr[i]);//***PRINT TO LCD***
        }
      
      stringStart++;
      stringStop++;      
     }
  }
}

//**************************************************************
//***********Function to Scroll Row 0 on LCD*******************
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
