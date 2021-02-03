
// Can find this in Github: https://github.com/rocketscream/Low-Power
// If you need a reference for IR Codes: http://lirc.sourceforge.net/remotes/sony/RM-S325
#include "LowPower.h"
#include <IRremote.h>

#define VOL_UP 1153
#define VOL_DOWN 3201
#define TAPE1 3137
#define TAPE2 577
#define AUX 2945
#define CD 2625
#define TUNER 2113
#define PHONO 65
#define NUM_OF_CHANNELS 6

// Timer Indicator
int counter;

// Amp Config
int channels[NUM_OF_CHANNELS] = {TAPE1,TAPE2,AUX,CD,TUNER,PHONO}; 
int pos = 0;

// Pin Layout
const int WAKE_UP_PIN = 2;
const int IR_PIN = 3;
const int VOL_UP_PIN = 7;
const int VOL_DOWN_PIN = 8;
const int LEFT_MOV_PIN = 9;
const int RIGHT_MOV_PIN = 10;

// IR Lib
IRsend irsend;


void setup() {
  Serial.begin(115200);

  // IR Stuff
  pinMode(VOL_UP_PIN, INPUT_PULLUP);
  pinMode(VOL_DOWN_PIN, INPUT_PULLUP);
  pinMode(RIGHT_MOV_PIN, INPUT_PULLUP);
  pinMode(LEFT_MOV_PIN, INPUT_PULLUP);
  pinMode(IR_PIN, OUTPUT);
  

  // Wake up Stuff
  pinMode(WAKE_UP_PIN, INPUT_PULLUP);

  // TIMER STUFF -> send interrupt every aprox ~ 1sec
  counter = 0;
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= (1<<CS12);
  TIMSK1 |= (1<<TOIE0);

}

ISR(TIMER1_OVF_vect)  {
  counter++;
}

void wakeUp() {
  Serial.println("Waking up");
  counter = 0;
}

void keepAlive() {
  counter = 0;  
}

void moveRight() {
   pos = (pos+1)%(NUM_OF_CHANNELS);
   sendCommand(channels[pos]);
   delay(100);
}

void moveLeft() {
   pos = (pos-1)%(NUM_OF_CHANNELS);
   pos = pos < 0 ? (NUM_OF_CHANNELS-1) : pos; 
   sendCommand(channels[pos]);
   delay(100);
}

void sendCommand(int code) {
    irsend.sendSony(code,12);
    delay(10);
    irsend.sendSony(code,12);
    delay(10);
    irsend.sendSony(code,12);
}

void loop() {

  attachInterrupt(0, wakeUp, HIGH);

  // If counter reaches 10 seconds, power down, else wake up
  if (counter >= 10) {
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  }

  detachInterrupt(0);

  // Read Digital Pins
  int vol_up = digitalRead(VOL_UP_PIN);
  int vol_down = digitalRead(VOL_DOWN_PIN);
  int mov_right = digitalRead(RIGHT_MOV_PIN);
  int mov_left = digitalRead(LEFT_MOV_PIN);

  // Check status of pins and send IR command as needed
  if (vol_up == LOW) {
    keepAlive();
    sendCommand(VOL_UP);
  } else if (vol_down == LOW) {
    keepAlive();
    sendCommand(VOL_DOWN); 
  } else if (mov_right == LOW) {
    keepAlive();
    moveRight();  
  } else if (mov_left == LOW) {
    keepAlive();
    moveLeft();  
  }

  
}
