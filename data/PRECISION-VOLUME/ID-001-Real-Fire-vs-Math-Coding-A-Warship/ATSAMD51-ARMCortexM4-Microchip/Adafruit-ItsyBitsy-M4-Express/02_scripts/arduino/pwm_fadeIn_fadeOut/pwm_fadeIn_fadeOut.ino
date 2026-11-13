#include <Arduino.h>

// Define your 14 target pins
const int ledPins[14] = {12, 11, 10, 9, 7, 5, 1, 0, 4, 2, 25, 24, 19, 18};

void setup() {
  // Initialize all pins as outputs
  for (int i = 0; i < 14; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
}

void loop() {
  // Example: Fade all 14 channels up simultaneously using hardware PWM
  for (int brightness = 0; brightness <= 255; brightness++) {
    for (int i = 0; i < 14; i++) {
      analogWrite(ledPins[i], brightness);
    }
    delay(10); // Adjust speed of fade
  }

  // Fade down
  for (int brightness = 255; brightness >= 0; brightness--) {
    for (int i = 0; i < 14; i++) {
      analogWrite(ledPins[i], brightness);
    }
    delay(10);
  }
}