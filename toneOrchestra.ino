#include <EEPROM.h>

//ID
int ID;
//IO
int tonePin = 7;
int masterStepPin[] = {11, 12, 13};
int slaveStepPin = 8;
// pattern initialiser
int sequenceLength = 16;
int beatCounter = 0;
int pattern[16];
int pattern1[] = {150, 150, 0, 150, 150, 150, 0, 150, 130, 150, 0, 130, 130, 150, 0, 130};
int pattern2[] = {1,0,0,5,4,5,1,0,1,1,0,5,4,5,2,3};
int pattern3[] = {120,0,140,5,108,5,120,0,1,120,0,101,4,106,2,172};
int pattern4[] = {5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5};
//tempo
double bpm = 100.0;
long interval;
//timer and sequencer
int cursor = 0;
unsigned long previousMillis = 0;
bool LED = false;
bool next = true;
bool stepState = LED;
bool play;
// scale, 10 tone scale, 8 octaves probably
int toneCount = 10;
long scale[80];


void setup() {
  //IOs
  pinMode(LED_BUILTIN, OUTPUT);
  for (int i = 0; i < 3; i++) {
    pinMode(masterStepPin[i], OUTPUT);
  }
  pinMode(slaveStepPin, INPUT);
  pinMode(slaveStepPin, INPUT);
  randomSeed(analogRead(0));
  Serial.begin(9600);
  //tempo
  interval = 14400.0/bpm; 
  //ID Assignment & play/pause reset toggle
  ID = EEPROM.read(0);
  play = EEPROM.read(1);
  EEPROM.write(1, !play);
  Serial.print("ID: ");
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
  // create the scale
  createScale(); 
  // startup glitch sweep
  int offset = random(20);
  int glitchy;
  for (int i = 10000 + offset; i > offset; i--) {
    glitchy = random(-50, 50);
    tone(tonePin, i + glitchy, 50);
  }
  noTone(tonePin);
//  for (int i = 0; i < 80; i++) {
//    Serial.println(scale[i]);
//    tone(tonePin, scale[i], 200);
//    delay(200);
//  }
}

void createScale() {
  Serial.println("Scale Creation:");
  int base = 68;
  int index = 0;
  for (long i = base; i <= (base * pow(2, 8)); i*=2) {
    int interval = i/(2*toneCount);
    for (int j = 0; j < toneCount; j++) {
      int note = (i/2) + (j * interval);
      if (index < 80) {
        scale[index] = note;
        Serial.print(scale[index]);
        Serial.print("\t");
        index++;
      } else {
        Serial.println("overflow");
        break;
      }
    }
    Serial.println();
  }
}

void loop() {
  //timer for sequencer
  if (ID == 1 && play == true) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      digitalWrite(LED_BUILTIN, LED);
      for (int i = 0; i < 3; i++) {
        digitalWrite(masterStepPin[i], LED); // use same LED flag to toggle step
      }
      LED = !LED;
      next = true;
    }
  }
  //slave sync
  if (ID != 1) {
    bool in = digitalRead(slaveStepPin);
    if (in != stepState) {
      next = true;
      stepState = in;
    }
  }

  //sound selection logic. anything below 100 reserved for drums, 100+ is scale index
  if (next == true) {
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
       int note = pattern[cursor] - 100;
        tone(tonePin, scale[note], 50);
    }
    cursor++; // get ready for the next step in the sequence
    if (cursor >= sequenceLength) {cursor = 0;}
    next = false;
    // SEQUENCE EXPERIMENTS GO HERE SORTOF
    beatCounter++; //keep track of how many beats, trigger events/ blah
    if (beatCounter%64 == 0){randomisePercussion(200);}

    //END SEQUENCE EXPERIMENTS
  }
}

bool coin() {
  return bool(random(2));
}

void randomisePercussion(int thresh) {
  for (int i = 0; i < sequenceLength; i++) {
    if (pattern[i] < thresh && coin()&& coin() == true) {
      pattern[i] = int(random(0, 6));
    }
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
    int glitchy = random(-50, 50);
    tone(tonePin, i + glitchy, 10);
  }
  noTone(tonePin);
}

void hiTom() {
  for (int i = 1534; i > 1034; i--) {
    int glitchy = random(-50, 50);
    tone(tonePin, i + glitchy, 10);
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

void assignPattern(int pat[] ) {
  for (int i = 0; i < sequenceLength; i++) {
    pattern[i] = pat[i];
    // hack to test scale
//    pattern[i] = random(100, 180);
  }
}

