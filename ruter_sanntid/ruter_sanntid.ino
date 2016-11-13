#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>
#include "WifiInfo.h"

SoftwareSerial esp8266(9,10);
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
WifiInfo wifiInfo;

#define DEBUG true

// WIFI and Serial
const int SERIAL_BAUD_RATE = 9600;
//const String WIFI_SSID = "kh.wifi";
//const String WIFI_PASSWORD = "";

// LCD constants
const int ROW_1 = 0;
const int ROW_2 = 1;
const int ROW_3 = 2;
const int ROW_4 = 3;

// Station constants
const String STATION_HOLBERG = "Holbergs Plass";
const String STATION_HOLBERG_ID = "3010057";
const String STATION_JERNBANETORGET = "Jernbanetorget";
const String STATION_JERNBANETORGET_ID = "3010011";

// Norwegian letter (å)
byte aa[8] = {
      B01110,
      B00000,
      B11110,
      B00010,
      B11110,
      B10010,
      B11110,
      B00000
};

void setup() {
  initSerial();
  initLcd();
  connectWifi();
}

void initSerial() {
  Serial.begin(SERIAL_BAUD_RATE);
  esp8266.begin(SERIAL_BAUD_RATE);
}

void initLcd() {
  lcd.createChar(0, aa);
  lcd.begin(20, 4);
}

void connectWifi() {
  lcd.print("Connecting");
  digitalWrite(12, HIGH);
  sendData("AT+RST\r\n", 5000, DEBUG);
  lcd.print(".");
  sendData("AT+CWMODE=1\r\n", 1000, DEBUG);
  lcd.print(".");
  sendData("AT+CWJAP=\"" + wifiInfo.ssid()+ "\",\"" + wifiInfo.password() + "\"\r\n", 8000, DEBUG);
  lcd.clear();
  lcd.print("Connected");
}

void loop() {
  station(STATION_HOLBERG, STATION_HOLBERG_ID);
  delay(2000);
  station(STATION_JERNBANETORGET, STATION_JERNBANETORGET_ID);
  delay(2000);
}

void station(String stationName, String stationId) {
  lcdPrint(stationName, ROW_1, true);
  String sanntidDataResponse = getSanntid(stationId);
  
  int startIndex = sanntidDataResponse.indexOf("[");
  int endIndex = sanntidDataResponse.indexOf("]");

  String json = sanntidDataResponse.substring(startIndex, endIndex+1);

  char buf[json.length()+1];
  json.toCharArray(buf, json.length()+1);

  StaticJsonBuffer<135> jsonBuffer;
  JsonArray& sanntidData = jsonBuffer.parseArray(buf);
 
  if (!sanntidData.success()) {
    if (DEBUG) {
      Serial.println("parseObject() failed");
    }
    closeConnection();
    return;
  }
  
  boolean first = true;

  String all = "";
  for(JsonArray::iterator it=sanntidData.begin(); it!=sanntidData.end(); ++it) {
    JsonObject& sanntid = *it;

    if (first) {
      first = false;
      printLcd(sanntid, ROW_2);

    } else {
      String line = sanntid["line"];
      String time = sanntid["time"];
      String destination = sanntid["d"];
      String tot = line + " " + destination + ": " + time + "  ";
      lcd.setCursor(0, ROW_3);
      lcd.print("                ");

      for (int len = 1; len < tot.length(); len++) {
        lcd.setCursor(len-1, ROW_3);
        lcd.print(tot.substring(len-1, len));
        delay(120);
      }
      delay(3000);
      for (int len = tot.length(); len >= 0; len--) {
        lcd.setCursor(len, ROW_3);
        lcd.print(" ");
        delay(120);
      }
      delay(2000);
    }
  }
  closeConnection();
  lcd.clear();
}

void printLcd(JsonObject& sanntid, int row) {
  String line = sanntid["line"];
  String time = sanntid["time"];
  String destination = sanntid["d"];

  lcd.setCursor(0,row);
  if (time == "xx") {
    lcd.print(line + " " + destination + ": ");
    lcd.print("n");
    lcd.print("\0");
    lcd.print("    ");
  } else {
    lcd.print(line + " " + destination + ": " + time + "    ");
  }
}

String sendData(String command, const int timeout, boolean debug) {
    String response = "";
    esp8266.print(command);
    long int time = millis();
    while( (time+timeout) > millis()) {
      while(esp8266.available()) {
        char c = esp8266.read();
        response+=c;
      }
    }
    
    if(debug) {
      Serial.println(response);
    }
    
    return response;
}

String sendData2(String command, const int timeout, boolean debug) {
    boolean inProgress = false;
    String response = "";
    esp8266.print(command);
    long int time = millis();

    char startMarker = '[';
    char endMarker = ']';

    while( (time+timeout) > millis()) {
      while(esp8266.available()) {
        char c = esp8266.read();
        
        if (inProgress == true) {
          if (c != endMarker) {
            response+=c;
          } else {
            inProgress = false; 
            response+=c; 
          }
        } else if (c == startMarker) {
            inProgress = true;
            response+=c;
        }
      }
    }
    
    if(debug) {
      Serial.println(response);
    }
    
    return response;
}

void lcdPrint(String& str, int row) {
  lcdPrint(str, row, false);
}

void lcdPrint(String& str, int row, boolean clear) {
  if (clear) {
    lcd.clear();
  }
  lcd.setCursor(0, row);
  lcd.print(str);
}

String getSanntid(String& stationId) {
  sendData("AT+CIPSTART=\"TCP\",\"simple-ruter-api.herokuapp.com\",80\r\n",1000, DEBUG);
  sendData("AT+CIPSEND=63\r\n", 1000, DEBUG);
  sendData("GET /"+stationId+" HTTP/1.1\r\n", 1000, DEBUG);
  sendData("Host: simple-ruter-api.herokuapp.com\r\n", 1000, DEBUG);
  return sendData2("\r\n", 5000, DEBUG);
}

void closeConnection() {
  sendData("AT+CIPCLOSE\r\n", 1000, DEBUG);
}

