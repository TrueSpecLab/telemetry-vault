// --- PIN DEFINITIONS ---
// Create as array that will store the LED gpio pin numbers
const uint8_t ledPins[] = {
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

const uint8_t triggerPin = 32; // Connect a button from this pin to GND

// Potentiometer pins (ESP32 ADC pins)
const uint8_t potIdlePin = 25;      // Adjusts idle flicker intensity
const uint8_t potChaosPin = 26;     // Adjusts chaotic amplitude
const uint8_t potHarmonicPin = 27;  // Adjusts the duration/damping of the harmonic ringing
const uint8_t potBaselinePin = 33;  // Adjusts the overall baseline brightness

// --- PWM SETTINGS ---
const uint16_t freq = 5000;
const uint8_t resolution = 12; // 0-4095

// --- STATE MACHINE ---
enum FlameState { IDLE, DROP, CHAOTIC, HARMONIC };
FlameState currentState = IDLE;

unsigned long stateStartTime = 0;
unsigned long currentTime = 0;

// --- IDLE FLICKER VARIABLES ---
uint16_t currentIdlePWM = 1000;  // Will quickly snap to actual baseline on startup
uint16_t targetIdlePWM = 1000;
unsigned long lastIdleChange = 0;


// Sharp drop to near-zero, followed by a fast 150ms recovery up to the baseline
const uint16_t dropDuration = 150;

// Violent random swings that decay over 600ms
const uint16_t chaosDuration = 600;

void setup() {
  Serial.begin(115200);
  while(!Serial);
  //delay(1000);
  
  for(uint8_t i = 0; i < 14; i++){
    // Configure LED pins as OUTPUTs
    pinMode(ledPins[i], OUTPUT);

    // Drive OUTPUTs LOW
    digitalWrite(ledPins[i], LOW);

    // configure LED PWMs
    ledcAttach(ledPins[i], freq, resolution);
  }

  // Configure trigger pin as input
  pinMode(triggerPin, INPUT);
  
  // Seed the random number generator
  randomSeed(analogRead(36)); 
}

void loop() {
  currentTime = millis();
  
  // --- READ TRIM POTS ---
  // Map the 0-4095 ADC readings to useful parameter ranges
  uint16_t idleIntensity = map(analogRead(potIdlePin), 0, 4095, 50, 800);
  uint16_t chaosAmplitude = map(analogRead(potChaosPin), 0, 4095, 500, 2000);
  float harmonicDamping = map(analogRead(potHarmonicPin), 0, 4095, 10, 50) / 1000.0; 
  
  // New baseline brightness pot (mapped from a dim 200 up to a bright 3500)
  uint16_t baselinePWM = map(analogRead(potBaselinePin), 0, 4095, 200, 3500);

  //uint16_t outputPWM = 2000; // Baseline brightness
  uint16_t outputPWM = baselinePWM;
  unsigned long timeInState = currentTime - stateStartTime;

  // --- TRIGGER LOGIC ---
  // If button is pressed and we are currently IDLE, trigger the disturbance
  if (digitalRead(triggerPin) == HIGH && currentState == IDLE) {
    currentState = DROP;
    stateStartTime = currentTime;
    delay(50); // Simple debounce
  }

  // --- STATE MACHINE LOGIC ---
  switch (currentState) {
    
    case IDLE:
    {
      // Generate a smooth "drunken walk" flicker
      if (currentTime - lastIdleChange > random(20, 80)) {
        targetIdlePWM = baselinePWM + random(-idleIntensity, idleIntensity);
        lastIdleChange = currentTime;
      }
      // Smoothly move current PWM toward target PWM
      currentIdlePWM += (targetIdlePWM - currentIdlePWM) * 0.2;
      outputPWM = currentIdlePWM;
      //Serial.println("outputPWM = " + String(outputPWM));
    }
      break;

    case DROP:
    {      
      if (timeInState < dropDuration) {
        outputPWM = map(timeInState, 0, dropDuration, 100, 2000);
        // outputPWM = map(timeInState, 0, dropDuration, 100, baselinePWM);
      } else {
        currentState = CHAOTIC;
        stateStartTime = currentTime;
      }
    }
      break;

    case CHAOTIC:
    {
      if (timeInState < chaosDuration) {
        // Calculate decaying envelope (1.0 down to 0.0)
        float envelope = 1.0 - ((float)timeInState / chaosDuration);
        uint16_t currentChaosAmp = chaosAmplitude * envelope;
        outputPWM = 2000 + random(-currentChaosAmp, currentChaosAmp);
      } else {
        currentState = HARMONIC;
        stateStartTime = currentTime;
      }
      Serial.println("chaosAmplitude = " + String(chaosAmplitude));
    }
      break;

    case HARMONIC:
    {
      // Damped sine wave ringing
      uint16_t harmonicDuration = 2000; // Max time in this state

      if (timeInState < harmonicDuration) {
        float timeSec = timeInState / 1000.0;
        float frequency = 8.0; // 8 Hz oscillation
        
        // Apply the damped sine wave formula
        float oscillation = sin(2 * PI * frequency * timeSec);
        float damping = exp(-harmonicDamping * timeInState);

        Serial.println("harmonicDamping = " + String(harmonicDamping));
        
        outputPWM = 2000 + (int)(chaosAmplitude * 0.5 * damping * oscillation);
        
        // Return to idle if the damping has essentially zeroed out the oscillation
        if (damping < 0.05) {
          currentState = IDLE;
          currentIdlePWM = outputPWM; // Seamlessly hand off
        }
      } else {
        currentState = IDLE;
      }
    }
    break;
  }

  // Constrain to 12-bit bounds before writing
  outputPWM = constrain(outputPWM, 0, 4095);
  
  // Apply a basic gamma curve for visual realism (squaring)
  // Normalized 0.0-1.0, squared, then mapped back to 0-4095
  float normalized = outputPWM / 4095.0;
  uint16_t gammaPWM = (uint16_t)((normalized * normalized) * 4095);

  ledcWrite(ledPins[0], gammaPWM);

  /*for(uint8_t i = 0; i < 14; i++){
    ledcWrite(ledPins[i], gammaPWM);
    //delay(10);
  }*/
  
  delay(10); // Loop pacing (~100Hz to match your original recording)
}