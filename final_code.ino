#include <BH1750FVI.h>
#include "DHT.h"
#define dhtPin 12
#define dhtType DHT22

#include <LiquidCrystal_PCF8574.h>
#include <Wire.h>
#include <Keypad.h>

BH1750FVI LightSensor(BH1750FVI::k_DevModeContLowRes);
DHT dht(dhtPin, dhtType);

int  humidity, temperature , fanSpeed = 255 , fogSpeed = 150                // Value
                                        , fanPinA = 22, fanPinB = 23, fogPinA = 24, fogPinB = 25, fanPWM = 10, fogPWM = 11         // Drive motor pin
                                            , motorPin1 = 2, motorPin2 = 3, ledPin = 4        // Relay pin
                                                , lowHumidity = 70, highHumidity = 75, lowTemperature = 24, highTemperature = 27 // Range of value
                                                    , buzzerPin = 13, infraredPin = 14        // Module Pin
                                                        , infraredStatus ;                       // Infrared Status
long lux , lowLux = 50 , highLux = 60;
char humidityStatus, temperatureStatus, luxStatus   ;
int curtainRoll = 1;
// Status


LiquidCrystal_PCF8574 lcd(0x3F);
char key;   // input key
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {37, 36, 35, 34};
byte colPins[COLS] = {33, 32, 31, 30};
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );     //setting keypad


void setup() {
  Serial.begin(9600);
  pinMode(fanPWM, OUTPUT); //drive
  pinMode(fanPinA, OUTPUT);
  pinMode(fanPinB, OUTPUT);
  pinMode(fogPinA, OUTPUT);
  pinMode(fogPinB, OUTPUT);
  pinMode(motorPin1, OUTPUT); //relay
  pinMode(motorPin2, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, HIGH);
  LightSensor.begin();
  dht.begin();
  lcd.setBacklight(255);
  // put your setup code here, to run once:
  lcd.begin(20, 4);
  lcd.display();
  lcd.setBacklight(255);
  lcd.home();
  lcd.clear();
  //  firstSetCurtain();

}

void loop() {

  sensorRead();
  valueStatus();
  lcdShow();
  condition();

  key = keypad.getKey();
  checkButtonA();
  checkButtonB();
  checkButtonC();
  trueRange();
  Serial.print("curtainRoll :");
  Serial.println(curtainRoll);
  //  digitalWrite(fanPinA, HIGH);
  //  digitalWrite(fanPinB, LOW);
  //  analogWrite(pwm1, 255);
  //  digitalWrite(2, HIGH);
  //  delay(10000);
  //  analogWrite(10, 50);
  //  digitalWrite(2, LOW);
  //  delay(10000);

}

void Beep() {
  digitalWrite(buzzerPin, LOW); delay(50);
  digitalWrite(buzzerPin, HIGH);
}


void sensorRead() {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  lux = LightSensor.GetLightIntensity();
  infraredStatus = digitalRead(infraredPin);
  delay(500);
  //Serial.println(highHumidity);
  //  Serial.print("Humidity:\t");
  //  Serial.print(humidity);
  //  Serial.print("% \t");
  //  Serial.print("Temperature:\t");
  //  Serial.print(temperature);
  //  Serial.print("\tC \t");
  //  Serial.print("Light:\t");
  //  Serial.print(lux);
  //  Serial.println("\tlux");
}

void condition() {
  if  ((luxStatus == 'L') && (temperatureStatus == 'L') && (humidityStatus == 'L' )) {           //1
    turnOffFan();
    turnOnFog();
    ledOn();
    turnOffCurtain();
    Serial.println("1: L L L ");
  }
  else if  ((luxStatus == 'H') && (temperatureStatus == 'H') && (humidityStatus == 'H' )) {           //2
    turnOnFan();
    turnOffFog();
    ledOff();
    turnOnCurtain();
    Serial.println("2: H H H ");
  }
  else if ((luxStatus == 'N') && (temperatureStatus == 'N') && (humidityStatus == 'N' )) {           //3
    turnOffFan();
    turnOffFog();
    ledOff();
    turnOffCurtain();
    Serial.println("3: N N N ");
  }
  else if ((luxStatus == 'L') && (temperatureStatus == 'L') && (humidityStatus == 'H' )) {           //4
    turnOnFan();
    turnOffFog();
    ledOn();
    turnOffCurtain();
    Serial.println("4: L L H ");
  }
  else if ((luxStatus == 'L') && (temperatureStatus == 'H') && (humidityStatus == 'N' )) {           //5
    turnOnFan();
    turnOffFog();
    ledOn();
    turnOffCurtain();
    Serial.println("5: L H N ");
  }
  else if ((luxStatus == 'L') && (temperatureStatus == 'N') && (humidityStatus == 'H' )) {           //6
    turnOnFan();
    turnOffFog();
    ledOn();
    turnOffCurtain();
    Serial.println("6: L N H ");
  }
  else if ((luxStatus == 'L') && (temperatureStatus == 'L') && (humidityStatus == 'N' )) {          //7
    turnOffFan();
    turnOffFog();
    ledOn();
    turnOffCurtain();
    Serial.println("7: L L N ");
  }
  else if ((luxStatus == 'L') && (temperatureStatus == 'H') && (humidityStatus == 'H' )) {           //8
    turnOnFan();
    turnOffFog();
    ledOn();
    turnOffCurtain();
    Serial.println("8: L H H ");
  }
  else if ((luxStatus == 'L') && (temperatureStatus == 'N') && (humidityStatus == 'N' )) {           //9
    turnOffFan();
    turnOffFog();
    ledOn();
    turnOffCurtain();
    Serial.println("9: L N N ");
  }
  else if ((luxStatus == 'L') && (temperatureStatus == 'H') && (humidityStatus == 'L' )) {           //10
    turnOnFan();
    turnOnFog();
    ledOn();
    turnOffCurtain();
    Serial.println("10: L H L ");
  }
  else if ((luxStatus == 'L') && (temperatureStatus == 'N') && (humidityStatus == 'L' )) {           //11
    turnOffFan();
    turnOnFog();
    ledOn();
    turnOffCurtain();
    Serial.println("11: L N L ");
  }
  else if ((luxStatus == 'H') && (temperatureStatus == 'L') && (humidityStatus == 'L' )) {           //12
    turnOffFan();
    turnOnFog();
    ledOff();
    turnOnCurtain();
    Serial.println("12: H L L ");
  }
  else if ((luxStatus == 'H') && (temperatureStatus == 'H') && (humidityStatus == 'N' )) {           //13
    turnOnFan();
    turnOffFog();
    ledOff();
    turnOnCurtain();
    Serial.println("13: H H N ");
  }
  else if ((luxStatus == 'H') && (temperatureStatus == 'N') && (humidityStatus == 'L' )) {           //14
    turnOffFan();
    turnOnFog();
    ledOff();
    turnOnCurtain();
    Serial.println("14: H N L ");
  }
  else if ((luxStatus == 'H') && (temperatureStatus == 'L') && (humidityStatus == 'N' )) {           //15
    turnOffFan();
    turnOffFog();
    ledOn();
    turnOnCurtain();
    Serial.println("15: H L N ");
  }
  else if ((luxStatus == 'H') && (temperatureStatus == 'H') && (humidityStatus == 'L' )) {           //16
    turnOnFan();
    turnOnFog();
    ledOff();
    turnOnCurtain();
    Serial.println("16: H H L ");
  }
  else if ((luxStatus == 'H') && (temperatureStatus == 'N') && (humidityStatus == 'N' )) {           //17
    turnOffFan();
    turnOffFog();
    ledOff();
    turnOnCurtain();
    Serial.println("17: H N N ");
  }
  else if ((luxStatus == 'H') && (temperatureStatus == 'L') && (humidityStatus == 'H' )) {           //18
    turnOffFan();
    turnOnFog();
    ledOn();
    turnOnCurtain();
    Serial.println("18: H L H ");
  }
  else if ((luxStatus == 'H') && (temperatureStatus == 'N') && (humidityStatus == 'H' )) {           //19
    turnOnFan();
    turnOffFog();
    ledOff();
    turnOnCurtain();
    Serial.println("19: H N H ");
  }
  else if ((luxStatus == 'N') && (temperatureStatus == 'L') && (humidityStatus == 'L' )) {           //20
    turnOffFan();
    turnOnFog();
    ledOn();
    turnOffCurtain();
    Serial.println("20: N L L ");
  }
  else if ((luxStatus == 'N') && (temperatureStatus == 'H') && (humidityStatus == 'H' )) {           //21
    turnOnFan();
    turnOffFog();
    ledOff();
    turnOffCurtain();
    Serial.println("21: N H H ");
  }
  else if ((luxStatus == 'N') && (temperatureStatus == 'N') && (humidityStatus == 'L' )) {           //22
    turnOffFan();
    turnOnFog();
    ledOff();
    turnOffCurtain();
    Serial.println("22: N H H ");
  }
  else  if ((luxStatus == 'N') && (temperatureStatus == 'L') && (humidityStatus == 'H' )) {           //23
    turnOffFan();
    turnOnFog();
    ledOn();
    turnOffCurtain();
    Serial.println("23: N L H ");
  }
  else if ((luxStatus == 'N') && (temperatureStatus == 'H') && (humidityStatus == 'L' )) {           //24
    turnOnFan();
    turnOnFog();
    ledOff();
    turnOffCurtain();
    Serial.println("24: N H L ");
  }
  else if ((luxStatus == 'N') && (temperatureStatus == 'N') && (humidityStatus == 'H' )) {           //25
    turnOnFan();
    turnOffFog();
    ledOff();
    turnOffCurtain();
    Serial.println("25: N N H ");
  }
  else if ((luxStatus == 'N') && (temperatureStatus == 'L') && (humidityStatus == 'N' )) {           //26
    turnOffFan();
    turnOffFog();
    ledOff();
    turnOffCurtain();
    Serial.println("26: N L N ");
  }
  else if ((luxStatus == 'N') && (temperatureStatus == 'H') && (humidityStatus == 'N' )) {           //27
    turnOnFan();
    turnOffFog();
    ledOff();
    turnOffCurtain();
    Serial.println("27: N H N ");
  }
  else {
    Serial.println("NO ");
  }
}

void valueStatus() {

  if ((humidity >= lowHumidity) && (humidity <= highHumidity)) {
    humidityStatus = 'N';
    Serial.println("humidityStatus = N");
  }

  if (humidity < lowHumidity ) {
    humidityStatus = 'L';
    Serial.println("humidityStatus = L");
  }

  if (humidity > highHumidity ) {
    humidityStatus = 'H';
    Serial.println("humidityStatus = H");

  }

  if ((temperature >= lowTemperature) && (temperature <= highTemperature)) {
    temperatureStatus = 'N';
    Serial.println("temperatureStatus = N");
  }
  if (temperature < lowTemperature) {
    temperatureStatus = 'L';
    Serial.println("temperatureStatus = L");
  }
  if (temperature > highTemperature) {
    temperatureStatus = 'H';
    Serial.println("temperatureStatus = H");
  }

  if ((lux >= lowLux) && ( lux <= highLux)) {
    luxStatus = 'N';
    Serial.println("luxStatus = N");
  }
  if (lux < lowLux) {
    luxStatus = 'L';
    Serial.println("luxStatus = L");
  }
  if (lux > highLux) {
    luxStatus = 'H';
    Serial.println("luxStatus = H");
  }
}

void turnOnFan () {
  digitalWrite(fanPinA, HIGH);
  digitalWrite(fanPinB, LOW);
  analogWrite(fanPWM, fanSpeed);

}

void turnOffFan () {
  digitalWrite(fanPinA, HIGH);
  digitalWrite(fanPinB, HIGH);
  analogWrite(fanPWM, 0);
}

void turnOnFog() {
  digitalWrite(fogPinA, HIGH);
  digitalWrite(fogPinB, LOW);
  analogWrite(fogPWM, fogSpeed);
}

void turnOffFog() {
  digitalWrite(fogPinA, HIGH);
  digitalWrite(fogPinB, HIGH);
  analogWrite(fogPWM, 0);
}

void turnOnCurtain() {          //close      //ม่านปิดอยู่ ม่าานทำงาน
  if (curtainRoll == 1) {
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, HIGH);
    delay(4000);
//    if (infraredStatus == 0) {
//      digitalWrite(motorPin1, LOW);
//      digitalWrite(motorPin2, LOW);
//
//    }
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);


  }
  curtainRoll = 0;
}
void turnOffCurtain() {       //open      //ม่านเปิดอยู่ ม่าานม้วน
  if (curtainRoll == 0) {
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);
    delay(4000);
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);

  }
  curtainRoll = 1;
}
void firstSetCurtain() {
  infraredStatus = digitalRead(infraredPin);
  if (infraredStatus == 0) {     //have curtiain
    digitalWrite(motorPin1, HIGH);            // sec
    digitalWrite(motorPin2, LOW);
    delay(4000);
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
  }
  curtainRoll = "1";
}

void ledOn() {
  digitalWrite(ledPin, LOW);
}

void ledOff() {
  digitalWrite(ledPin, HIGH);
}

void lcdShow() {
  lcd.setCursor(0, 0);
  lcd.print("Humi = ");
  lcd.setCursor(8, 0);
  lcd.print(humidity);
  lcd.setCursor(11, 0);
  lcd.print("%");
  lcd.setCursor(13, 0);
  lcd.print("[");
  lcd.setCursor(14, 0);
  lcd.print(lowHumidity);   //14,0
  lcd.setCursor(16, 0);
  lcd.print("-");
  lcd.setCursor(17, 0);
  lcd.print(highHumidity);  //17,0
  lcd.setCursor(19, 0);
  lcd.print("]");

  lcd.setCursor(0, 1);
  lcd.print("Temp = ");
  lcd.setCursor(8, 1);
  lcd.print(temperature);
  lcd.setCursor(11, 1);
  lcd.print("C");
  lcd.setCursor(13, 1);
  lcd.print("[");
  lcd.setCursor(14, 1);
  lcd.print(lowTemperature);  //14,1
  lcd.setCursor(16, 1);
  lcd.print("-");
  lcd.setCursor(17, 1);
  lcd.print(highTemperature); //17,1
  lcd.setCursor(19, 1);
  lcd.print("]");

  lcd.setCursor(0, 2);
  lcd.print("L = ");
  lcd.setCursor(4, 2);
  lcd.print(lux);
  lcd.print(" lux ");
  lcd.setCursor(0, 3);
  lcd.print("[");
  lcd.setCursor(1, 3);
  lcd.print(lowLux);
  lcd.setCursor(6, 3);
  lcd.print("-");
  lcd.setCursor(7, 3);
  lcd.print(highLux);
  lcd.setCursor(12, 3);
  lcd.print("]");
}
void checkButtonA() {

  if (key == 'A') {
    int N = 0; int arrayLowStoreA[2]; int arrayHighStoreA[2];   //s    key
    Beep();
    int LowStoreA = 0;   //v
    int HighStoreA = 0;  //u
    lcd.blink();
    lcd.setCursor(14, 0);
    while (key != '#') {
      key = NO_KEY;

      while (key == NO_KEY ) {
        key = keypad.getKey();
      }
      if (key >= '0' && key <= '9') {
        if (N < 2) {
          Beep();
          arrayLowStoreA[N] = key - 48;
          lcd.setCursor(14 + N, 0);
          lcd.print(arrayLowStoreA[N]);
          N++;
        }
      }

    }
    for (int i = 0 ; i < 2; i++) {
      if (arrayLowStoreA[i] > 9 || arrayLowStoreA[i] < 0) {
        arrayLowStoreA[i] = 0;
      }
    }

    for (int i = 0 ; i < 2; i++) {
      if (i == 0) {
        LowStoreA += arrayLowStoreA[i] * 10;
      }
      if (i == 1) {
        LowStoreA += arrayLowStoreA[i] * 1;
      }
    }
    Beep();
    lowHumidity = LowStoreA;
    N = 0;
    lcd.setCursor(17, 0);
    key = NO_KEY;

    while (key != '#') {
      key = NO_KEY;

      while (key == NO_KEY ) {
        key = keypad.getKey();
      }
      if (key >= '0' && key <= '9') {
        if (N < 2) {
          Beep();
          arrayHighStoreA[N] = key - 48;
          lcd.setCursor(17 + N, 0);
          lcd.print(arrayHighStoreA[N]);
          N++;
        }
      }

    }
    for (int i = 0 ; i < 2; i++) {
      if (arrayHighStoreA[i] > 9 || arrayHighStoreA[i] < 0) {
        arrayHighStoreA[i] = 0;
      }
    }

    for (int i = 0 ; i < 2; i++) {
      if (i == 0) {
        HighStoreA += arrayHighStoreA[i] * 10;
      }
      if (i == 1) {
        HighStoreA += arrayHighStoreA[i] * 1;
      }
    }
    Beep();
    highHumidity = HighStoreA;
    lcd.noBlink();
    lcd.clear();
    key = NO_KEY;
  }

}
void checkButtonB() {

  if (key == 'B') {
    int N = 0; int arrayLowStoreB[2]; int arrayHighStoreB[2];   //s
    Beep();
    int LowStoreB = 0;   //v
    int HighStoreB = 0;   //u
    lcd.blink();
    lcd.setCursor(14, 1);
    while (key != '#') {
      key = NO_KEY;

      while (key == NO_KEY ) {
        key = keypad.getKey();
      }
      if (key >= '0' && key <= '9') {
        if (N < 2) {
          Beep();
          arrayLowStoreB[N] = key - 48;
          lcd.setCursor(14 + N, 1);
          lcd.print(arrayLowStoreB[N]);
          N++;
        }
      }

    }
    for (int i = 0 ; i < 2; i++) {
      if (arrayLowStoreB[i] > 9 || arrayLowStoreB[i] < 0) {
        arrayLowStoreB[i] = 0;
      }
    }

    for (int i = 0 ; i < 2; i++) {
      if (i == 0) {
        LowStoreB += arrayLowStoreB[i] * 10;
      }
      if (i == 1) {
        LowStoreB += arrayLowStoreB[i] * 1;
      }
    }
    Beep();
    lowTemperature = LowStoreB;
    N = 0;
    lcd.setCursor(17, 1);
    key = NO_KEY;

    while (key != '#') {
      key = NO_KEY;

      while (key == NO_KEY ) {
        key = keypad.getKey();

      }
      if (key >= '0' && key <= '9') {
        if (N < 2) {
          Beep();
          arrayHighStoreB[N] = key - 48;
          lcd.setCursor(17 + N, 1);
          lcd.print(arrayHighStoreB[N]);
          N++;
        }
      }

    }
    for (int i = 0 ; i < 2; i++) {
      if (arrayHighStoreB[i] > 9 || arrayHighStoreB[i] < 0) {
        arrayHighStoreB[i] = 0;
      }
    }

    for (int i = 0 ; i < 2; i++) {
      if (i == 0) {
        HighStoreB += arrayHighStoreB[i] * 10;
      }
      if (i == 1) {
        HighStoreB += arrayHighStoreB[i] * 1;
      }
    }
    Beep();
    highTemperature = HighStoreB;
    lcd.noBlink();
    lcd.clear();
    key = NO_KEY;
  }


}

void checkButtonC() {

  if (key == 'C') {
    int N = 0, count = 0; long power = 1;  long arrayLowStoreC[5]; long arrayHighStoreC[5];    // a = count  b = power w = arrayLowStoreC y = arrayHighStoreC
    Beep();
    long lowStoreC = 0;     // lowStoreC    v
    long highStoreC = 0;     // highStoreC  u
    lcd.blink();
    lcd.setCursor(1, 3);
    lcd.print("     ");
    lcd.setCursor(1, 3);
    for (int i = 0; i < 5; i++) {
      arrayLowStoreC[i] = -555555;
    }
    while (key != '#') {
      key = NO_KEY;

      while (key == NO_KEY ) {
        key = keypad.getKey();

      }
      if (key >= '0' && key <= '9') {
        if (N < 5) {
          Beep();
          arrayLowStoreC[N] = key - 48;
          lcd.setCursor(1 + N, 3);
          lcd.print(arrayLowStoreC[N]);
          N++;
        }
      }
    }
    for (int i = 0; i < 5; i++) {
      if (arrayLowStoreC[i] > 9 || arrayLowStoreC[i] < 0) {
        count++;
        arrayLowStoreC[i] = 0;
      }
    }

    for (int i = 0 ; i < 5; i++) {
      if (i == 0) {
        lowStoreC += arrayLowStoreC[i] * 10000;
      }
      if (i == 1) {
        lowStoreC += arrayLowStoreC[i] * 1000;
      }
      if (i == 2) {
        lowStoreC += arrayLowStoreC[i] * 100;
      }
      if (i == 3) {
        lowStoreC += arrayLowStoreC[i] * 10;
      }
      if (i == 4) {
        lowStoreC += arrayLowStoreC[i] * 1;
      }
    }

    for (int i = 0 ; i < count ; i++) {
      power *= 10;
    }

    lowStoreC /= power;
    lowLux = lowStoreC;
    Beep();
    N = 0;
    count = 0;
    power = 1;
    lcd.clear();
    lcdShow();

    lcd.setCursor(7, 3);
    lcd.print("     ");
    lcd.setCursor(12, 3);
    lcd.print("]");
    lcd.setCursor(7, 3);

    for (int i = 0; i < 5; i++) {
      arrayHighStoreC[i] = -555555;
    }

    key = NO_KEY;
    while (key != '#') {
      key = NO_KEY;
      while (key == NO_KEY ) {
        key = keypad.getKey();

      }
      if (key >= '0' && key <= '9') {
        if (N < 5) {
          Beep();
          arrayHighStoreC[N] = key - 48;
          lcd.setCursor(7 + N, 3);
          lcd.print(arrayHighStoreC[N]);
          N++;

        }
      }
    }
    for (int i = 0; i < 5; i++) {

      if (arrayHighStoreC[i] > 9 || arrayHighStoreC[i] < 0) {
        count++;
        arrayHighStoreC[i] = 0;
      }
    }

    for (int i = 0 ; i < 5; i++) {
      if (i == 0) {
        highStoreC += arrayHighStoreC[i] * 10000;
      }
      if (i == 1) {
        highStoreC += arrayHighStoreC[i] * 1000;
      }
      if (i == 2) {
        highStoreC += arrayHighStoreC[i] * 100;
      }
      if (i == 3) {
        highStoreC += arrayHighStoreC[i] * 10;
      }
      if (i == 4) {
        highStoreC += arrayHighStoreC[i] * 1;
      }
    }

    for (int i = 0 ; i < count ; i++) {
      power *= 10;

    }
    highStoreC /= power;

    highLux = highStoreC;
    Beep();
    lcd.noBlink();
    lcd.clear();
    lcdShow();
    key = NO_KEY;
  }


}

void trueRange() {
  if (lowHumidity >= highHumidity) {
    lowHumidity = highHumidity - 5;
    lcd.clear();
  }
  if (lowTemperature >= highTemperature) {
    lowTemperature = highTemperature - 5;
    lcd.clear();
  }
  if (lowLux >= highLux) {
    lowLux = highLux - 5;
    lcd.clear();
  }
}
