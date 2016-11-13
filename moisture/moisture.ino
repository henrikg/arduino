#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int LIGHT_SENSOR = A3;
const int MOISTURE_SENSOR = A0;

const int MOISTURE_HIGH = 12;
const int MOISTURE_OK = 11;
const int MOISTURE_LOW = 13;

const int MOISTURE_TRESHOLD_LOW = 800;
const int MOISTURE_TRESHOLD_OK = 600;

const int BAUD = 9600;

// Set the pins on the I2C chip used for LCD connections:
// addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

void setup() {
  initPins();
  initLcd();
  
  Serial.begin(BAUD);
}

void loop() {
  int moistureValue = analogRead(MOISTURE_SENSOR);
  int lightValue = analogRead(LIGHT_SENSOR);
  Serial.println(moistureValue); 
  lcd.clear();
  lcd.print("Moisture: ");
  lcd.print(moistureValue);
  lcd.setCursor(0,1);
  lcd.print("Light: ");
  lcd.print(lightValue);
  
  if (moistureValue > MOISTURE_TRESHOLD_LOW) {
    lowMoisture();
  } else if (moistureValue > MOISTURE_TRESHOLD_OK) {
    okMoisture();
  } else {
    highMoisture();
  }
  
  delay(1000);
}

void initPins() {
  pinMode(LIGHT_SENSOR, INPUT);
  pinMode(MOISTURE_SENSOR, INPUT);
  pinMode(MOISTURE_HIGH, OUTPUT);
  pinMode(MOISTURE_OK, OUTPUT);
  pinMode(MOISTURE_LOW, OUTPUT);
}

void initLcd() {
  lcd.begin(20,4);
}

void highMoisture() {
  digitalWrite(MOISTURE_HIGH, HIGH);
  digitalWrite(MOISTURE_OK, LOW);
  digitalWrite(MOISTURE_LOW, LOW);
}

void okMoisture() {
  digitalWrite(MOISTURE_HIGH, LOW);
  digitalWrite(MOISTURE_OK, HIGH);
  digitalWrite(MOISTURE_LOW, LOW);
}

void lowMoisture() {
  digitalWrite(MOISTURE_HIGH, LOW);
  digitalWrite(MOISTURE_OK, LOW);
  digitalWrite(MOISTURE_LOW, HIGH);
}

