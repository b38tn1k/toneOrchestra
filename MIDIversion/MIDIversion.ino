// A very simple MIDI synth.
// Greg Kennedy 2011

#include <avr/pgmspace.h>

#define statusLed 13
#define tonePin 53

// MIDI channel to answer to, 0x00 - 0x0F
#define myChannel 0x00
// set to TRUE and the device will respond to all channels
#define respondAllChannels false

// midi commands
#define MIDI_CMD_NOTE_OFF 0x80
#define MIDI_CMD_NOTE_ON 0x90
#define MIDI_CMD_KEY_PRESSURE 0xA0
#define MIDI_CMD_CONTROLLER_CHANGE 0xB0
#define MIDI_CMD_PROGRAM_CHANGE 0xC0
#define MIDI_CMD_CHANNEL_PRESSURE 0xD0
#define MIDI_CMD_PITCH_BEND 0xE0

// this is a placeholder: there are
//  in fact real midi commands from F0-FF which
//  are not channel specific.
// this simple synth will just ignore those though.
#define MIDI_CMD_SYSEX 0xF0

// a dummy "ignore" state for commands which
//  we wish to ignore.
#define MIDI_IGNORE 0x00

// midi "state" - which data byte we are receiving
#define MIDI_STATE_BYTE1 0x00
#define MIDI_STATE_BYTE2 0x01

// MIDI note to frequency
//  This isn't exact and may sound a bit detuned at lower notes, because
//  the floating point values have been rounded to uint16.
//  Based on A440 tuning.

// I would prefer to use the typedef for this (prog_uint16_t), but alas that triggers a gcc bug
// and does not put anything into the flash memory.

// Also note the limitations of tone() which at 16mhz specifies a minimum frequency of 31hz - in other words, notes below
// B0 will play at the wrong frequency since the timer can't run that slowly!
uint16_t const frequency[128] PROGMEM = {8, 9, 9, 10, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 18, 19, 21, 22, 23, 24, 26, 28, 29, 31, 33, 35, 37, 39, 41, 44, 46, 49, 52, 55, 58, 62, 65, 69, 73, 78, 82, 87, 92, 98, 104, 110, 117, 123, 131, 139, 147, 156, 165, 175, 185, 196, 208, 220, 233, 247, 262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, 523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988, 1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976, 2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951, 4186, 4435, 4699, 4978, 5274, 5588, 5920, 5920, 6645, 7040, 7459, 7902, 8372, 8870, 9397, 9956, 10548, 11175, 11840, 12544};

//setup: declaring iputs and outputs and begin serial
void setup() {
  pinMode(statusLed, OUTPUT);  // declare the LED's pin as output

  pinMode(tonePin, OUTPUT);          // setup tone output pin
  pinMode(52, OUTPUT);
  digitalWrite(52, LOW);
  tone(tonePin, 500, 200);
  delay(500);

  //start serial with midi baudrate 31250
  // or 38400 for debugging (eg MIDI over serial from PC)
  Serial1.begin(31250);
  Serial.begin(9600);

  // indicate we are ready to receive data!
  digitalWrite(statusLed, HIGH);



}

bool drums = true;

double LFO;
double lfoCounter = 0;
int targetFreq = 0;
//int lfoDepth = 10;
int lfoDepth = 10;
unsigned int freq;
unsigned int freq2;
unsigned int freq3;
int analogRamp = 0;

//loop: wait for serial data
void loop () {
  static byte note;
  static byte lastCommand = MIDI_IGNORE;
  static byte state;
  static byte lastByte;
  LFO = cos(lfoCounter) * lfoDepth;
  lfoCounter += 0.5;
//  lfoCounter += 0.01;
//  Serial.println(LFO);

  while (Serial1.available()) {

    // read the incoming byte:
    byte incomingByte = Serial1.read();

    // Command byte?
    if (incomingByte & 0b10000000) {
      if (respondAllChannels ||
          (incomingByte & 0x0F) == myChannel) { // See if this is our channel
        lastCommand = incomingByte & 0xF0;
      } else { // Not our channel.  Ignore command.
        lastCommand = MIDI_IGNORE;
      }
      state = MIDI_STATE_BYTE1; // Reset our state to byte1.
    } else if (state == MIDI_STATE_BYTE1) { // process first data byte
      if ( lastCommand == MIDI_CMD_NOTE_OFF )
      { // if we received a "note off", make sure that is what is currently playing
        if (note == incomingByte) {
          noTone(tonePin);
          freq = 0;
        }
        state = MIDI_STATE_BYTE2; // expect to receive a velocity byte
      } else if ( lastCommand == MIDI_CMD_NOTE_ON ) { // if we received a "note on", we wait for the note (databyte)
        lastByte = incomingByte;  // save the current note
        state = MIDI_STATE_BYTE2; // expect to receive a velocity byte
      }
      // implement whatever further commands you want here
    } else { // process second data byte
      if (lastCommand == MIDI_CMD_NOTE_ON) {
        if (incomingByte != 0) {
          note = lastByte;
          Serial.println(note);
//          freq = (unsigned int)pgm_read_word(&frequency[note]);
//          freq2 = (unsigned int)pgm_read_word(&frequency[note+12]);
//          freq3 = (unsigned int)pgm_read_word(&frequency[note-12]);
          
          //DRUMS
          drums = false;
          if (drums == true) {
            switch (note) {
              case 36:
                kick1();
                break;
              case 37:
                ratchet1();
                break;
              case 38:
                ratchet2();
                break;
              case 39:
                click1();
                break;
              case 40:
                kick2();
                break;
              case 41:
                snare1();
                break;
              case 42:
                snare2();
                break;
              case 43:
                ratchet3();
                break;
              case 44:
                ratchet4();
                break;
              case 45:
                ratchet5();
                break;
              case 46:
                click2();
                break;
              case 47:
                click3();
                break;
              case 48:
                click4();
                break;
              case 49:
                click5();
                break;
              case 50:
                ratchet6();
                break;
              case 51:
                loTom();
                break;
              case 52:
                ratchet7();
                break;
              case 53:
                hiTom();
                break;
              case 54:
                hat();
                break;
            }
          }
        } else if (note == lastByte) {
          noTone(tonePin);
          freq = 0;
          analogRamp = 0;
        }
      }
      state = MIDI_STATE_BYTE1; // message data complete
      // This should be changed for SysEx
    }
  }
  tone(tonePin, freq);
  if (freq != 0 && drums == false) {
//    Serial.println(freq);
//    Serial.println(analogRamp);
//        // HATCHLING
//     analogWrite(8, analogRamp);
//     analogRamp += 1;
//    tone(tonePin, freq + LFO);
//    int duration = 15;
    // ARP
//    tone(tonePin, freq, duration);
//    delay(duration);
//    tone(tonePin, freq3 , duration);
//    delay(duration);
//    noTone(tonePin);
//    delay(1);
//    tone(tonePin, freq3, duration);
//    delay(duration);
    
    
//    //CHORDS
//    tone(tonePin, freq, duration);
//    delay(duration);
//    noTone(tonePin);
//    delay(duration);
//    tone(tonePin, freq, duration);
//    delay(duration);
//    noTone(tonePin);
//    delay(duration);
//    tone(tonePin, freq, duration);
//    tone(tonePin, freq, duration);
//    delay(duration);
//    noTone(tonePin);
//    delay(duration);
//    tone(tonePin, freq, duration);
//    delay(duration);
//    noTone(tonePin);
//    delay(duration);
//    tone(tonePin, freq, duration);
    //BASS
//    tone(tonePin, freq);
    // MELODY HIGH
//    tone(tonePin, freq + LFO);
  }
}

void kick1() {
  for (int i = 234; i > 34; i--) {
    tone(tonePin, i, 100);
    tone(tonePin, i, 100);
  }
  noTone(tonePin);
}

void kick2() {
  for (int i = 234; i > 34; i--) {
    tone(tonePin, i, 100);
    tone(tonePin, i, 100);
    tone(tonePin, i, 100);
    tone(tonePin, i, 100);
  }
  noTone(tonePin);
}

void click1() {
  for (int i = 234; i > 34; i--) {
    tone(tonePin, random(700, 800), 100);
  }
  noTone(tonePin);
}

void ratchet1() {
  for (int j = 0; j < 6; j++) {
    for (int i = 0; i < 7; i++) {
      tone(tonePin, random(1000, 1500), 100);
      delay(1);
    }
    noTone(tonePin);
    delay(40);
  }
}

void ratchet2() {
  for (int j = 0; j < 3; j++) {
    for (int i = 0; i < 5; i++) {
      tone(tonePin, random(1600, 2000), 100);
      tone(tonePin, random(1600, 2000), 100);
    }
    noTone(tonePin);
    noTone(tonePin);
    delay(40);
  }
}

void ratchet3() {
  for (int j = 0; j < 3; j++) {
    for (int i = 0; i < 7; i++) {
      tone(tonePin, random(1400, 5000), 100);
      delay(1);
    }
    noTone(tonePin);
    delay(60);
  }
}

void ratchet7() {
  for (int j = 0; j < 3; j++) {
    int lo = 500;
    int hi = 1000;
    for (int i = 0; i < 3; i++) {
      tone(tonePin, random(lo, hi), 100);
      lo += 100;
      hi += 100;
      delay(1);
    }
    noTone(tonePin);
    delay(60);
  }
}

void ratchet4() {
  int lo = 500;
  int hi = 1000;
  for (int j = 0; j < 3; j++) {
    int lo = 500;
    int hi = 1000;
    for (int i = 0; i < 6; i++) {
      tone(tonePin, random(lo, hi), 100);
      lo += 300;
      hi += 300;
      delay(1);
    }
    noTone(tonePin);
    delay(60);
  }
}

void click2() {
  for (int i = 0; i < 100; i++) {
    tone(tonePin, random(2000, 5999), 100);
  }
  noTone(tonePin);
}

void click3() {
  int hi = 6000;
  for (int i = 0; i < 50 ; i++) {
    tone(tonePin, random(2000, hi), 100);
    hi -= 60;
  }
  noTone(tonePin);
}

void click4() {
  int lo = 2000;
  for (int i = 0; i < 100; i++) {
    tone(tonePin, random(lo, 7000), 100);
    lo += 60  ;
  }
  noTone(tonePin);
}

void click5() {
  for (int i = 0; i < 50; i++) {
    tone(tonePin, random(2000, 5999), 100);
  }
  noTone(tonePin);
}

void ratchet6() {
  for (int k = 0; k < 4; k++) {
    for (int i = 0; i < 10; i++) {
      tone(tonePin, random(2000, 5999), 100);
    }
    for (int i = 0; i < 10; i++) {
      noTone(tonePin);
    }
  }
}

void ratchet5() {
  for (int j = 0; j < 3; j++) {
    int lo = 1000;
    int hi = 2000;
    for (int i = 0; i < 3; i++) {
      tone(tonePin, random(lo, hi), 1);
      lo += 1;
      hi += 10;
      delay(1);
    }
    noTone(tonePin);
    noTone(tonePin);
    noTone(tonePin);
    //    delay(5);
  }
}
void loTom() {
  for (int i = 700; i > 534; i--) {
    int glitchy = random(-50, 50);
    tone(tonePin, i + glitchy, 10);
  }
  noTone(tonePin);
}

void hiTom() {
  for (int i = 0; i < 1034; i++) {
    int glitchy = random(-50, 6000);
    tone(tonePin, i + glitchy, 10);
  }
  noTone(tonePin);
}

void snare1() {
  int lo = 500;
  for (int i = 400; i > 34; i--) {
    tone(tonePin, random(lo, 3000), 100);
    lo += 1;
  }
  noTone(tonePin);
}

void snare2() {
  int lo = 1000;
  for (int i = 300; i > 34; i--) {
    tone(tonePin, random(lo, 6000), 100);
    lo += 10;
  }
  noTone(tonePin);
}

void hat() {
  for (int i = 100; i > 0; i--) {
    tone(tonePin, random(3000, 6000), 1);
  }
  noTone(tonePin);
}
