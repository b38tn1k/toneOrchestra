#include <EEPROM.h>

int ID;
int tonePin = 7;
int pause = 200;
int pattern[16];
int pattern1[] = {500, 500, 0, 500, 500, 500, 0, 500, 300, 500, 0, 300, 300, 500, 0, 300};
int pattern2[] = {1,0,0,5,4,5,1,0,1,1,0,5,4,5,2,3};
int pattern3[] = {2000,0,400,5,800,5,2000,0,1,2000,0,1000,4,600,2,720};
int pattern4[] = {5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5};
double bpm = 100.0;
long interval;
int cursor = 0;
unsigned long previousMillis = 0;
bool LED = false;

int pitch = 2000;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  randomSeed(analogRead(0));
  interval = 14400.0/bpm; 
  pitch = random(1000, 3000);
  Serial.begin(9600);
  ID = EEPROM.read(0);
  Serial.println(ID);
  if (ID == 1) {
      assignPattern(pattern1);
  } else if (ID == 2) {
      assignPattern(pattern2);
  } else if (ID == 3) {
      assignPattern(pattern3);
  } else {
      assignPattern(pattern4);
  }
  int offset = random(20);
  for (int i = 10000 + offset; i > offset; i--) {
    tone(tonePin, i, 50);
  }
  noTone(tonePin);
}

void loop() {
  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    digitalWrite(LED_BUILTIN, LED);
    LED = !LED;
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    switch(pattern[cursor]) {
      case 0:
        noTone(tonePin);
        break;
      case 1:
        kick();
        break;
      case 2:
        hiTom();
        break;
      case 3:
        loTom();
        break;
      case 4:
        snare();
        break;
      case 5:
        hat();
        break;
       default:
        tone(tonePin, pattern[cursor], 50);
    }
    
    cursor++;
    if (cursor >= 16) {cursor = 0;}
  }
}

void kick() {
  for (int i = 234; i > 34; i--) {
    tone(tonePin, i, 100);
  }
  noTone(tonePin);
}

void loTom() {
  for (int i = 1034; i > 534; i--) {
    tone(tonePin, i, 10);
  }
  noTone(tonePin);
}

void hiTom() {
  for (int i = 1534; i > 1034; i--) {
    tone(tonePin, i, 10);
  }
  noTone(tonePin);
}

void snare() {
  for (int i = 534; i > 34; i--) {
    tone(tonePin, random(1000, 2000), 100);
  }
  noTone(tonePin);
}

void hat() {
  for (int i = 100; i > 0; i--) {
    tone(tonePin, random(3000, 6000), 1);
  }
  noTone(tonePin);
}

void setID(int id) {
  for (int addr = 0; addr < EEPROM.length(); addr++) {
    EEPROM.write(addr, id);
  }
}

void assignPattern(int pat[16] ) {
  for (int i = 0; i < 16; i++) {
    pattern[i] = pat[i];
  }
}

