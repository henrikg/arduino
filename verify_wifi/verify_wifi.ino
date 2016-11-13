/**
 * Verify and testing ESP8266
 */

#include <SoftwareSerial.h>
SoftwareSerial softSerial(9, 10); // RX, TX

void setup() {
  uint32_t baud = 9600;
  Serial.begin(baud);
  softSerial.begin(baud);
  Serial.print("Listening @");
  Serial.println(baud);
  Serial.println("If no response, change baud.");
}

void loop() {
    while(softSerial.available() > 0) {
      char a = softSerial.read();
      if(a == '\0')
        continue;
      if(a != '\r' && a != '\n' && (a < 32))
        continue;
      
      Serial.print(a);
    }
    
    while(Serial.available() > 0) {
      char a = Serial.read();
      Serial.write(a);
      softSerial.write(a);
    }
}
