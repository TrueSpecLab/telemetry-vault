#include <Arduino.h>

int pin = 25;

void setup() {
  pinMode(pin, OUTPUT);
}

void loop() {
  // Fade In (Breathing up)
  for (int i = 0; i <= 255; i++) {
    float normalized = (float)i / 255.0f;
    // Apply square-law gamma correction for smooth visual dimming
    int pwmValue = (int)(255.0f * (normalized * normalized));
    
    analogWrite(pin, pwmValue);
    delay(10);
  }

  // Fade Out (Breathing down)
  for (int i = 255; i >= 0; i--) {
    float normalized = (float)i / 255.0f;
    int pwmValue = (int)(255.0f * (normalized * normalized));
    
    analogWrite(pin, pwmValue);
    delay(10);
  }
}