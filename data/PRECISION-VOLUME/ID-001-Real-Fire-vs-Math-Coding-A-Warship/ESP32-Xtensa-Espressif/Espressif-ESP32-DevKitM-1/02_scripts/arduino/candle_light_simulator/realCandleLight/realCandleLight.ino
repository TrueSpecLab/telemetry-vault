#include "candle_data.h"

const int ledPin = 21;  // Choose your ESP32 PWM pin
const int freq = 5000;  // 5kHz is a good frequency for LEDs
const int ledChannel = 0;
const int resolution = 12; // 12-bit resolution (0-4095)

// Create as array that will store the gpio pin numbers
uint8_t gpio_pins[] = {
  GPIO_NUM_21,
  GPIO_NUM_22,
  GPIO_NUM_19,
  GPIO_NUM_23,
  GPIO_NUM_18,
  GPIO_NUM_5,
  GPIO_NUM_10,
  GPIO_NUM_9,
  GPIO_NUM_4,
  GPIO_NUM_0,
  GPIO_NUM_2,
  GPIO_NUM_15,
  GPIO_NUM_13,
  GPIO_NUM_12
  };

void setup() {
  for(uint8_t i = 0; i < 14; i++){
    // Set pin as OUTPUT
    pinMode(gpio_pins[i], OUTPUT);

    // Drive OUTPUT LOW
    digitalWrite(gpio_pins[i], LOW);

    // configure LED PWM
    ledcAttach(gpio_pins[i], freq, resolution);
  }
}

void loop() {
  // Iterate through the recorded data
  for(int i = 0; i < candleDataLength; i++) {
    ledcWrite(gpio_pins[0], candleData[i]);
    
    // The original data is sampled every 0.01 seconds (100Hz)
    delay(10); 
  }
}