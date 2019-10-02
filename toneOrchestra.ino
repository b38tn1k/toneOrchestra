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
bool mute = true;
int muteTime;
bool gen = true;


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
//  makePatterns();
  makeBand();
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
  muteTime = (ID - 1) * 16;
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
    Serial.print("ID: ");
    Serial.println(ID);
    Serial.print("Next mute change: ");
    Serial.println(muteTime);
    Serial.print("Beat: ");
    Serial.println(beatCounter);
    Serial.print("Mute state: ");
    Serial.println(mute);
    if (beatCounter >= muteTime) {
      mute = !mute;
      if (ID == 1) {
        if (mute == true) {
          muteTime += 16*2;
        } else {
          muteTime += 16*3;
        }
      } else if (ID == 2) {
        if (mute == true) {
          muteTime += 16*2;
        } else {
          muteTime += 16*7;
        }
      } else if (ID == 3) {
        if (mute == true) {
          muteTime += 16*2;
        } else {
          muteTime += 16*4;
        }
        
      } else if (ID == 4) {
        if (mute == true) {
          muteTime += 16*2;
        } else {
          muteTime += 16*6;
        }
      }
      noTone(tonePin);
    }
    if (gen == false) {mute = false;}
    if (mute == false) {
      switch(pattern[cursor]) {
        Serial.print("Event: ");
        Serial.println(pattern[cursor]);
        case 0:
          noTone(tonePin);
          break;
        case 1:
          kick();
          break;
        case 2:
          hat();
          break;
        case 3:
          snare();
          break;
        case 4:
          hiTom();
          break;
        case 5:
          loTom();
          break;
         default:
          int note = pattern[cursor] - 100;
          int duration;
          if (ID == 1) {
            duration = 20;
          } else if (ID == 2) {
            duration = interval;
          }
          tone(tonePin, scale[note], duration);
      }
    } else {
      noTone(tonePin);
    }
    cursor++; // get ready for the next step in the sequence
    if (cursor >= sequenceLength) {cursor = 0;}
    next = false;
    
    // SEQUENCE EXPERIMENTS GO HERE SORTOF
    beatCounter++; //keep track of how many beats, trigger events/ blah
    if (beatCounter%64 == 0){   randomisePercussion(99, true);    }
    if (beatCounter%96==0) {    newMelody();                      }
    //END SEQUENCE EXPERIMENTS
    Serial.println();
  }
}

bool coin() {
  return bool(random(2));
}

void randomisePercussion(int thresh, bool preserveZeros) {
  for (int i = 0; i < sequenceLength; i++) {
    if ((pattern[i] < thresh && coin()&& coin() == true) && !(preserveZeros == true and pattern[i] == 0)) {
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

void newMelody() {
  if (ID == 1) {
    for (int i = 0; i < sequenceLength; i++) {
      pattern[i] = int(random(140, 180));
      if(coin() && coin() && coin()) { pattern[i] = 0;}
    }
  } else if (ID == 2) {
    for (int i = 0; i < sequenceLength; i++) {
      pattern[i] = int(random(100, 140));
      if(coin() && coin() && coin()) { pattern[i] = 0;}
    }
  } else if (ID == 3) {
    for (int i = 0; i < sequenceLength; i+=2) {
      int val = int(random(139, 150));
      pattern[i] = val;
      pattern[i+1] = val;
      if(coin() && coin() && coin()) { pattern[i] = 0;}
      if(coin() && coin() && coin()) { pattern[i+1] = 0;}
    }
  }
}

void makeBand() {
  int hi, lo;
    if (ID == 1) {
      hi = 180;
      lo = 140;
  } else if (ID == 2) {
      hi = 140;
      lo = 100;
  } else if (ID == 3) {
      hi = 150;
      lo = 139;
  } else {
      hi = 4;
      lo = 0;
  }

  for (int i = 0; i < sequenceLength; i++) {
      pattern[i] = int(random(lo, hi));
      if(coin() && coin() && coin()) { pattern[i] = 0;}
  }
  if (ID == 3) {
    for (int i = 0; i < sequenceLength; i+=2) {
      int val = int(random(lo, hi));
      pattern[i] = val;
      pattern[i+1] = val;
      if(coin() && coin() && coin()) { pattern[i] = 0;}
      if(coin() && coin() && coin()) { pattern[i+1] = 0;}
  }
    
  }
    
}

void makePatterns() { 
  if (ID == 1) {
      assignPattern(pattern1);
  } else if (ID == 2) {
      assignPattern(pattern2);
  } else if (ID == 3) {
      assignPattern(pattern3);
  } else {
      assignPattern(pattern4);
  }
}

void assignPattern(int pat[] ) { 
  for (int i = 0; i < sequenceLength; i++) {
    pattern[i] = pat[i];
  }
}

void createScale() {
  Serial.println("Scale Creation:");
  int base = 68;
  int index = 0;
  for (long i = base; i <= (base * pow(2, 8)); i*=2) {
    int interval = i/(2*toneCount);
    for (int j = 0; j < toneCount/2; j++) {
      int note = (i/2) + (j * interval);
      if (index < 79) {
        scale[index] = note;
        scale[index+1] = note;
        Serial.print(scale[index + 1]);
        Serial.print("\t");
        Serial.print(scale[index]);
        Serial.print("\t");
        index+=2;
      } else {
        Serial.println("overflow");
        break;
      }
    }
    Serial.println();
  }
}

