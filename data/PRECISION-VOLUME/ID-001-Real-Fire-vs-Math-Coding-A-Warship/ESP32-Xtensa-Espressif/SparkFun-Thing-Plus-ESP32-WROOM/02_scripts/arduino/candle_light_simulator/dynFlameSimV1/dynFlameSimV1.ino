// --- PIN DEFINITIONS ---
const int triggerPin = 33; // Connect a button from this pin to GND

// Potentiometer pins (Master Adjustments)
const uint8_t potIdlePin = 36;      // Adjusts idle flicker intensity
const uint8_t potChaosPin = 39;     // Adjusts chaotic amplitude
const uint8_t potHarmonicPin = 34;  // Adjusts the duration/damping of the harmonic ringing
const uint8_t potBaselinePin = 25;  // Adjusts the overall baseline brightness

// --- MULTI-LED SETUP ---
const int NUM_FLAMES = 14;
// Example pinout for 8 LEDs
const int ledPins[NUM_FLAMES] = {21, 17, 16, 19, 18, 5, 4, 27, 12, 23, 22, 14, 32, 15};

const int freq = 5000;
const int resolution = 12; // 0-4095

enum FlameState { IDLE, DROP, CHAOTIC, HARMONIC };
enum Location { STERN, CABIN, BINNACLE, BATTLE, KITCHEN };

// --- THE FLAME CLASS ---
class Flame {
  public:
    int ledPin;
    FlameState currentState = IDLE;
    Location locationType;
    unsigned long stateStartTime = 0;
    
    float currentIdlePWM;
    float targetIdlePWM;
    unsigned long lastIdleChange = 0;
    
    // Internal base recipe values specific to this location
    int baseBaseline;
    int baseIdleIntensity;
    int baseChaosAmplitude;
    float baseHarmonicDamping;
    
    // "Personality" variables to make each LED unique
    int brightnessOffset; 
    float flickerSpeedMod;
    float chaosMod;

    void init(int pin, Location loc) {
      ledPin = pin;
      locationType = loc;
      ledcAttach(ledPin, freq, resolution);
      
      // Generate unique personality for this specific lantern
      brightnessOffset = random(-200, 200); 
      flickerSpeedMod = random(80, 120) / 100.0; // 0.8x to 1.2x speed
      chaosMod = random(80, 120) / 100.0;        // 0.8x to 1.2x chaos intensity

      // Load the historical recipe based on the location
      switch (locationType) {
        case STERN:
          baseBaseline = 3500;
          baseIdleIntensity = 800;
          baseChaosAmplitude = 1500;
          baseHarmonicDamping = 0.010; // Low damping: bounces around a lot
          break;
        case CABIN:
          baseBaseline = 3000;
          baseIdleIntensity = 100;
          baseChaosAmplitude = 800;
          baseHarmonicDamping = 0.040; // High damping: settles very fast
          break;
        case BINNACLE:
          baseBaseline = 1000;
          baseIdleIntensity = 20;
          baseChaosAmplitude = 200;
          baseHarmonicDamping = 0.050; // Max damping: highly protected, stiff flame
          break;
        case BATTLE:
          baseBaseline = 2000;
          baseIdleIntensity = 400;
          baseChaosAmplitude = 1000;
          baseHarmonicDamping = 0.025; // Medium stats
          break;
        case KITCHEN:
          baseBaseline = 4000;
          baseIdleIntensity = 1500;
          baseChaosAmplitude = 2000;
          baseHarmonicDamping = 0.001; // Minimum damping: chaotic fire that never truly settles
          break;
      }
      
      currentIdlePWM = baseBaseline;
      targetIdlePWM = baseBaseline;
    }

    void update(unsigned long currentTime, float masterBaselineScale, float masterIdleScale, float masterChaosScale, float masterHarmonicScale) {
      
      // Calculate this LED's specific values using its base recipe, personality mods, and global master scales
      int myBasePWM = constrain((baseBaseline * masterBaselineScale) + brightnessOffset, 50, 4095);
      int myIdleInt = baseIdleIntensity * masterIdleScale * chaosMod;
      int myChaosAmp = baseChaosAmplitude * masterChaosScale * chaosMod;
      float myHarmDamp = baseHarmonicDamping * masterHarmonicScale;
      
      int outputPWM = myBasePWM; 
      unsigned long timeInState = currentTime - stateStartTime;

      switch (currentState) {
        case IDLE:
          if (currentTime - lastIdleChange > random(20 * flickerSpeedMod, 80 * flickerSpeedMod)) {
            targetIdlePWM = myBasePWM + random(-myIdleInt, myIdleInt);
            lastIdleChange = currentTime;
          }
          currentIdlePWM += (targetIdlePWM - currentIdlePWM) * 0.2;
          outputPWM = currentIdlePWM;

          break;

        case DROP:
          if (timeInState < 150) {
            outputPWM = map(timeInState, 0, 150, 100, myBasePWM);
          } else {
            currentState = CHAOTIC;
            stateStartTime = currentTime;
          }
          break;

        case CHAOTIC:
          if (timeInState < 600) {
            float envelope = 1.0 - ((float)timeInState / 600.0);
            int currentChaos = myChaosAmp * envelope;
            outputPWM = myBasePWM + random(-currentChaos, currentChaos);
          } else {
            currentState = HARMONIC;
            stateStartTime = currentTime;
          }
          break;

        case HARMONIC:
          if (timeInState < 2000) {
            float timeSec = timeInState / 1000.0;
            float frequency = 8.0 * flickerSpeedMod; // 8 * flickerSpeedMod Hz oscillation
            
            // Apply the damped sine wave formula
            float oscillation = sin(2 * PI * frequency * timeSec);
            float damping = exp(-myHarmDamp * timeInState); 
            
            outputPWM = myBasePWM + (int)(myChaosAmp * 0.5 * damping * oscillation);
            
            if (damping < 0.05) {
              currentState = IDLE;
              currentIdlePWM = outputPWM;
            }
          } else {
            currentState = IDLE;
          }
          break;
      }

      // Constrain to 12-bit bounds before writing
      outputPWM = constrain(outputPWM, 0, 4095);

      // Apply a basic gamma curve for visual realism (squaring)
      // Normalized 0.0-1.0, squared, then mapped back to 0-4095
      float normalized = outputPWM / 4095.0;
      int gammaPWM = (int)((normalized * normalized) * 4095);

      ledcWrite(ledPin, gammaPWM);
    }

    void triggerDrop(unsigned long time) {
      if (currentState == IDLE) {
        currentState = DROP;
        stateStartTime = time;
      }
    }
};

// Create an array of our Flame objects
Flame flames[NUM_FLAMES];

// --- WAVE SEQUENCER VARIABLES ---
bool waveActive = false;
unsigned long waveStartTime = 0;
int currentWaveIndex = 0;
int battleLanternsTriggered = 0;
const int waveDelayMS = 80; 

void setup() {
  Serial.begin(115200);
  while(!Serial);

  pinMode(triggerPin, INPUT);

  randomSeed(analogRead(26)); 

  // Initialize your 14 LEDs and assign them to specific locations on your model
  flames[0].init(ledPins[0], STERN);     // 1 Stern Lantern
  flames[1].init(ledPins[1], CABIN);     // 1 Great Cabin
  flames[2].init(ledPins[2], BINNACLE);  // 1 Binnacle
  flames[3].init(ledPins[3], KITCHEN);   // 1 Galley Fire
  flames[4].init(ledPins[4], KITCHEN);
  flames[5].init(ledPins[5], KITCHEN);
  flames[6].init(ledPins[6], BATTLE);    // 8 Battle Lanterns
  flames[7].init(ledPins[7], BATTLE);
  flames[8].init(ledPins[8], BATTLE);
  flames[9].init(ledPins[9], BATTLE);
  flames[10].init(ledPins[10], BATTLE);
  flames[11].init(ledPins[11], BATTLE);
  flames[12].init(ledPins[12], BATTLE);
  flames[13].init(ledPins[13], BATTLE);
}

void loop() {
  unsigned long currentTime = millis();
  
  // Read potentiometers and map them to a 0.0 to 2.0 multiplier scale (0% to 200%)
  float masterBaselineScale = map(analogRead(potBaselinePin), 0, 4095, 0, 200) / 100.0;
  float masterIdleScale =     map(analogRead(potIdlePin),     0, 4095, 0, 200) / 100.0;
  float masterChaosScale =    map(analogRead(potChaosPin),    0, 4095, 0, 200) / 100.0;
  float masterHarmonicScale = map(analogRead(potHarmonicPin), 0, 4095, 0, 200) / 100.0; 

  // --- TRIGGER THE WAVE ---
  if (digitalRead(triggerPin) == HIGH && !waveActive) {
    waveActive = true;
    waveStartTime = currentTime;
    currentWaveIndex = 0;
    battleLanternsTriggered = 0; // Reset our battle-only counter
  }

// --- PROCESS THE WAVE ---
  if (waveActive) {
    if (currentWaveIndex < NUM_FLAMES) {
      
      // If this specific light is NOT a battle lantern, skip it instantly
      if (flames[currentWaveIndex].locationType != BATTLE) {
        currentWaveIndex++;
      }
      // If it IS a battle lantern, wait for the correct wave timing to trigger it
      else if (currentTime > waveStartTime + (battleLanternsTriggered * waveDelayMS)) {
        flames[currentWaveIndex].triggerDrop(currentTime);
        currentWaveIndex++;
        battleLanternsTriggered++;
      }
      
    } else {
      // Once we have checked all LEDs in the array, end the wave
      waveActive = false;
    }
  }

  // --- UPDATE ALL FLAMES ---
  for (int i = 0; i < NUM_FLAMES; i++) {
    flames[i].update(currentTime, masterBaselineScale, masterIdleScale, masterChaosScale, masterHarmonicScale);
  }
  
  delay(10); // Loop pacing
}