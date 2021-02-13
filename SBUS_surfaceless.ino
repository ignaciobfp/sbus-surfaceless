#define DEBUG_MODE 1
#include "SBUS.h"
#include <Servo.h> // include the servo library to control the servos
#define PWM_CENTER 1500
#define PWM_RANGE 1000
// a SBUS object, which is on hardware serial port 2
SBUS sb_flightcontroller(Serial2);

Servo servo1;   // name each servo output for use with the servo library
Servo servo2;

const int servo1_pin = 16;  // identify the pins that each servo signal wire is connected to
const int servo2_pin = 17;

const int16_t ail1_direction = 1;
const int16_t prof1_direction = -1;
const int16_t ail2_direction = 1;
const int16_t prof2_direction = 1;

const int16_t ail_downscale = 4;
const int16_t prof_downscale = 4;

boolean CURRENT_MODE_IS_COPTER = true;

const uint16_t fm_ranges[7] = {0, 1231, 1361, 1491, 1621, 1750, 2200};
const boolean fm_iscopter[7] = {false, false, true, false, false, true};

// channel, fail safe, and lost frames data
uint16_t channels[16];
bool failSafe;
bool lostFrame;


inline float usToMovePct(uint16_t us) {
  return ((float)(us) - PWM_CENTER) / PWM_RANGE;
}

inline int16_t usToMoveUs(uint16_t us) {
  return ((int16_t) us - PWM_CENTER);
}

inline uint16_t trimToValidUs(int16_t us) {
  if (us > 2150) return 2150;
  else if (us < 850) return 850;
  else return (uint16_t) us;
}

void setup() {
  if(DEBUG_MODE)
    Serial.begin(9600);
  // begin the SBUS communication
  sb_flightcontroller.begin();
  // attach the servos
  servo1.attach(servo1_pin);
  servo2.attach(servo2_pin);
}

void loop() {
  // look for a good SBUS packet from the receiver
  if (sb_flightcontroller.read(&channels[0], &failSafe, &lostFrame)) {
    sbusToUsVector();
    #if DEBUG_MODE
      printChannels();
    #endif
    uint16_t servo1_uS;      // variables to store the pulse widths to be sent to the servo
    uint16_t servo2_uS;

    //check flight mode
    uint16_t channel_fm = channels[12]; // copied fligth mode to channel 12 (any other would work) since sbus output does not retain this info
    for (unsigned char i=0; i < 6; ++i){
      if(channel_fm >= fm_ranges[i] && channel_fm < fm_ranges[i+1]){
          CURRENT_MODE_IS_COPTER = fm_iscopter[i];
          break;
      }
    }

    if (!CURRENT_MODE_IS_COPTER) {
      servo1_uS = trimToValidUs(channels[2]
                                + ail1_direction * usToMoveUs(channels[13]) / (2 * ail_downscale)
                                + prof1_direction * usToMoveUs(channels[14]) / (2 * prof_downscale));
      servo2_uS = trimToValidUs(channels[3]
                                + ail2_direction * usToMoveUs(channels[13]) / (2 * ail_downscale)
                                + prof2_direction * usToMoveUs(channels[14]) / (2 * prof_downscale));
    }
    else {
      servo1_uS = channels[2];
      servo2_uS = channels[3];
    }
    servo1.writeMicroseconds(servo1_uS);   // write the pulsewidth to the servo.
    servo2.writeMicroseconds(servo2_uS);   // write the pulsewidth to the servo.
  }
}

void printChannels(){
  for (int i = 0; i < 16; ++i){
    Serial.print("Channel ");
    Serial.print(i);
    Serial.print(" - ");
    Serial.println(channels[i]);
  }
}

uint16_t sbusToUs(uint16_t sbus_value){
  //Sbus goes from 0 to 2048, must be converted to a valid us range like 1000-2000
  //Most RX define min as 172 and max to 1811
  const uint16_t OldMin = 172;
  const uint16_t OldMax = 1811;
  const uint16_t NewMin = 990;
  const uint16_t NewMax = 2010; 
  return (((sbus_value - OldMin) * (NewMax - NewMin)) / (OldMax - OldMin)) + NewMin;
}

void sbusToUsVector(){
    for (unsigned char i=0; i < 16; ++i){
      channels[i] = sbusToUs(channels[i]);
    }
}
