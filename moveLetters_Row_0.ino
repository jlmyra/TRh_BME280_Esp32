//**************************************************************
//***********Function to Scroll Row 0 on TFT*******************
//**************************************************************
// This function scrolls the date/time text from right to left on the TFT display.
// Adapted from the original LCD scrolling function.
// For TFT, we use pixel coordinates instead of character positions.
// The text scrolls at the top of the screen (row 0, y position 0-60)


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

//Begin TFT print Routine

    // Clear the top row area (0-30 pixels high)
    tft.fillRect(0, 0, TFT_WIDTH, 30, TFT_BLACK);

    // Calculate pixel position for scrolling
    // scrollCursor is in character positions, convert to pixels
    int xPos = scrollCursor * TFT_CHAR_WIDTH;

    tft.setCursor(xPos, 10); // Y position = 10 pixels from top
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);

//***-FIRST-'if' to print the first characters appearing from right

      if(stringStart == 0 && scrollCursor > 0) //***-FIRST-'if'
      {
      for (int i = 0; i < stringStop; i++)
        {
      tft.print (row0Ptr[i]); //***PRINT TO TFT***
        }
      scrollCursor--;
      stringStop++;
      }

 //***-SECOND-'if' End of line reached--reset everything

      else

      if (stringStart == stringStop) //***-SECOND-'if'
      {
      stringStart = stringStop = 0;
      scrollCursor = screenWidth;
      }

//*--THIRD-- if: Print the last characters of the line

  else

  if (stringStop == strlen(row0Ptr) && scrollCursor == 0) //*--THIRD-- if
  {
        for (int i = stringStart; i< stringStop; i++)
        {
        tft.print(row0Ptr[i]);//***PRINT TO TFT***
        }

      stringStart++;
  }
//---Fourth if: print next character chunks shifting the line by one character
      else
      {
        for (int i = stringStart; i< stringStop; i++)
        {
      tft.print(row0Ptr[i]);//***PRINT TO TFT***
        }

      stringStart++;
      stringStop++;
     }
  }
}

//**************************************************************
//***********Function to Scroll Row 0 on TFT*******************
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
