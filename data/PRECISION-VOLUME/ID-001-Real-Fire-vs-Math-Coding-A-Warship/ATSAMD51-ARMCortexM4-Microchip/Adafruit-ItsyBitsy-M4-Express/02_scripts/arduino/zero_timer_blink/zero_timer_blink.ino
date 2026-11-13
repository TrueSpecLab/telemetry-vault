#include <Arduino.h>
#include "Adafruit_ZeroTimer.h"

Adafruit_ZeroTimer zerotimer = Adafruit_ZeroTimer(3); // Use TC3

int pin = 4; // 12, 11, 10, 9, 7, 5, 21, 1, 0, 4, 3, 2, 23, 25

void TC3_Handler() {
  Adafruit_ZeroTimer::timerHandler(3);
}

void timerCallback(void) {
  // Code to run on every timer tick
  digitalWrite(pin, HIGH);
}

void setup() {
  pinMode(pin, OUTPUT);
  
  // Configure timer callback and frequency
  zerotimer.enable(false);

  // Set up frequency and attach timerCallback
  zerotimer.configure(TC_CLOCK_PRESCALER_DIV1024, 
                      TC_COUNTER_SIZE_16BIT, 
                      TC_WAVE_GENERATION_MATCH_FREQ);
  zerotimer.setCompare(0, 46875); // Adjust match value
  zerotimer.setCallback(true, TC_CALLBACK_CC_CHANNEL0, timerCallback);
  zerotimer.enable(true);
}

void loop() {
  delay(100);
  digitalWrite(pin, LOW);
}
