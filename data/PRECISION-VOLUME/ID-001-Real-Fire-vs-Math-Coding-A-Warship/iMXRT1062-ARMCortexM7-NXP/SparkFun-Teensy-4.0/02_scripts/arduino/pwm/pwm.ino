const uint8_t ledPin = 13;      // Pin 6 is backed by a FlexPWM module on Teensy 4.0
int brightness = 0;         // Current brightness (0 to 4095 for 12-bit resolution)
int fadeAmount = 20;        // Step size for smooth fading

void setup() {
  pinMode(ledPin, OUTPUT);
  
  // Configure 12-bit PWM resolution (0 to 4095) for fine-grained control
  analogWriteResolution(12);
  
  // Set the FlexPWM hardware timer frequency to 5 kHz 
  // (high enough to eliminate audible coil whine and visible flicker)
  analogWriteFrequency(ledPin, 5000);
}

void loop() {
  // Drive the pin using the underlying FlexPWM hardware timer
  analogWrite(ledPin, brightness);

  // Increment or decrement brightness
  brightness += fadeAmount;

  // Reverse direction at the 12-bit boundaries
  if (brightness <= 0 || brightness >= 4095) {
    fadeAmount = -fadeAmount;
  }

  // Short delay for smooth visual pacing
  delay(10);
}