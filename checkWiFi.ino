
void checkWiFi(){

    unsigned long startAttemptTime = millis();
    const unsigned long wifiTimeout = 30000; // 30 seconds timeout

    while(WiFi.status() != WL_CONNECTED &&
          (millis() - startAttemptTime) < wifiTimeout){

    Serial.print("Attempting to connect to wifi: "); Serial.println(ssid);

    WiFi.disconnect();

    delay(2000);

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("  Attempting WiFi   ");
    lcd.setCursor(0,1);
    lcd.print("   reconnection     ");

    WiFi.begin(ssid, password);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network

    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);

    delay(3000);

    if (WiFi.status() == WL_CONNECTED){
      lcd.clear();
    }
   }

   // Check if connection failed after timeout
   if (WiFi.status() != WL_CONNECTED) {
      Serial.println();
      Serial.println("ERROR: WiFi connection failed after timeout");
      Serial.println("Restarting ESP32...");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("WiFi Timeout!");
      lcd.setCursor(0,1);
      lcd.print("Restarting...");
      delay(2000);
      ESP.restart();
   }

   Serial.println();
    Serial.print("WiFi CONNECTED - IP:"); Serial.println(WiFi.localIP());
    //Serial.println(" Subnet Mask: "); Serial.print(WiFi.subnetMask());
    Serial.print(" MAC addr: "); Serial.println(WiFi.BSSIDstr());
    Serial.print(" Gateway IP: "); Serial.println(WiFi.gatewayIP());
    Serial.print(" Gateway DNS: "); Serial.println(WiFi.dnsIP(0)); 
    Serial.print(" SSID: "); Serial.println(WiFi.SSID());
    Serial.print(" Channel: "); Serial.println(WiFi.channel());
    Serial.print(" RSSI: "); Serial.println(WiFi.RSSI()); 
    Serial.println();
   }

  
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
 
