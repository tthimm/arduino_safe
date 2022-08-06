#include <Wire.h>
#include <LiquidCrystal_I2C.h>
// sda = A4
// scl = A5 

#include <Keypad.h>
// digital pins for keypad
const byte COLS = 4;
const byte ROWS = 4;
const byte colPins[COLS] = {5,  6,  7,  8};
const byte rowPins[ROWS] = {9, 10, 11, 12};

#include <Servo.h>
// orange = pwm datapin = D13
// brown  = gnd
// red    = vcc
Servo servo;
const byte servoPin     = 13;

 // servo angle
const byte servoClose  =  90;
const byte servoOpen    = 5;

LiquidCrystal_I2C lcd(0x27, 16, 2);

byte keylength = 0;
// state 0 = boot, 1 = closed, 2 = code entered, 3 = open
byte state = 0;
char password[3] = {'1', '5', '6'};
char userdata[3] = {};

const char hexaKeys[ROWS][COLS]={
{'D','#','0','*'},
{'C','9','8','7'},
{'B','6','5','4'},
{'A','3','2','1'}
};

char key; 
Keypad myKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

// led pins
const byte red   = 2;
const byte green = 3;
const byte blue  = 4;

void printToLCD(String str) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(str);
  lcd.setCursor(0, 1);
}

void servoMove(int angle) {
  servo.attach(servoPin);
  servo.write(angle);
  delay(500);
  servo.detach();
}

void ledColor(char c) {
  switch(c) {
    case 'r':
      digitalWrite(red, HIGH);
      digitalWrite(green, LOW);
      digitalWrite(blue, LOW);
      break;
    case 'g':
      digitalWrite(red, LOW);
      digitalWrite(green, HIGH);
      digitalWrite(blue, LOW);
      break;
    case 'b':
      digitalWrite(red, LOW);
      digitalWrite(green, LOW);
      digitalWrite(blue, HIGH);
      break;
  }
}

void setup() {
  Serial.begin(9600);
  lcd.init(); 
  lcd.backlight();
  lcd.cursor();
  //printToLCD("Schliessen mit C");
  //ledColor('g');
  //servoMove(servoOpen);
  servoMove(servoClose);
  state = 1;
}

void loop() {
  switch(state) {
    case 0:
      key = myKeypad.getKey();
      if (key && key == 'C') {
        servoMove(servoClose);
        lcd.write(key);
        delay(1000);
        state = 1;
      }
      break;
    case 1:
      printToLCD("Code eingeben:");
      ledColor('r');
      state = 2;
      break;
    case 2:
      key = myKeypad.getKey();
      if (key) {
        ledColor('b');
        delay(500);
        ledColor('r');
        
        userdata[keylength++] = key;
        
        Serial.println(keylength);
        lcd.write(key);
      }
      if (keylength == 3) {
        if (userdata[0] != password[0] || userdata[1] != password[1] || userdata[2] != password[2]) {
          printToLCD("Falscher Code!");
          delay(2000);
          state = 1;
        }
        else {
          state = 3;
        }
        keylength = 0;
      }
      break;
    case 3:
      ledColor('g');
      printToLCD("Safe ist offen!");
      
      // open safe
      servoMove(servoOpen);
      delay(1000);
      state = 4;
      break;
     case 4:
       printToLCD("Schliessen mit C");
       state = 0;
       break;
  }
}
