// Define your 14 target pins
//const int ledPins[14] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 24};
const int ledPins[14] = {PB9, PB8, PB7, PB6, PB5, PB4, PB3, PA15, PA11, PA10, PA9, PA8, PB10, PA3};

//const uint8_t ledPin = 13;  // Pin 6 is backed by a FlexPWM module on Teensy 4.0
int brightness = 0;         // Current brightness (0 to 4095 for 12-bit resolution)
int fadeAmount = 20;        // Step size for smooth fading

void setup() {
  // Configure 12-bit PWM resolution (0 to 4095) for fine-grained control
  //analogWriteResolution(12);

  // Initialize all pins as outputs
  for (int i = 0; i < (int)(sizeof(ledPins) / sizeof(ledPins[0])); i++) {
    pinMode(ledPins[i], OUTPUT);

    // Set the FlexPWM hardware timer frequency to 5 kHz 
    // (high enough to eliminate audible coil whine and visible flicker)
    //analogWriteFrequency(ledPins[i], 5000);
    analogWriteFrequency(5000);
  }
}

void loop() {
  // Example: Fade all 14 channels up simultaneously using hardware PWM
  for (int brightness = 0; brightness <= 255; brightness++) {
    for (int i = 0; i < (int)(sizeof(ledPins) / sizeof(ledPins[0])); i++) {
      analogWrite(ledPins[i], brightness);
    }
    delay(10); // Adjust speed of fade
  }

  // Fade down
  for (int brightness = 255; brightness >= 0; brightness--) {
    for (int i = 0; i < (int)(sizeof(ledPins) / sizeof(ledPins[0])); i++) {
      analogWrite(ledPins[i], brightness);
    }
    delay(10);
  }
}