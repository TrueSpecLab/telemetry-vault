// ==============================================================================
// 17TH CENTURY GALLEON FLAME SYNTHESIZER
// ==============================================================================

// --- PIN DEFINITIONS ---
const int triggerPin = 33;          // Connect a button from this pin to GND to trigger the "wind wave"

// Potentiometer pins (Master Adjustments - Use 3.3V, GND, and the Pin)
const uint8_t potIdlePin = 36;      // Adjusts how intensely the flame flickers while resting
const uint8_t potChaosPin = 39;     // Adjusts how violently the flame reacts to a gust of wind
const uint8_t potHarmonicPin = 34;  // Adjusts how quickly the flame settles back to normal after a gust
const uint8_t potBaselinePin = 25;  // Adjusts the overall master brightness of the ship
const uint8_t potSpeedPin = 26;     // Adjusts the Global Tempo (Wind Agitation / Physics Speed)

// --- MULTI-LED SETUP ---
const int NUM_FLAMES = 14;
const int ledPins[NUM_FLAMES] = {21, 17, 16, 19, 18, 5, 4, 27, 12, 23, 22, 14, 32, 15};

// --- PWM SETTINGS ---
const int freq = 5000;              // 5kHz frequency prevents visible LED strobing/flicker
const int resolution = 12;          // 12-bit resolution provides 4096 levels of brightness (0-4095)

enum FlameState { IDLE, DROP, CHAOTIC, HARMONIC };
enum Location { STERN, CABIN, BINNACLE, BATTLE, KITCHEN };

// ==============================================================================
// THE FLAME CLASS
// Represents a single, independent physical flame on the ship.
// ==============================================================================
class Flame {
  public:
    int ledPin;
    FlameState currentState = IDLE;
    Location locationType;
    unsigned long stateStartTime = 0; // Tracks when the current state began
    
    // Variables to ensure smooth gliding during IDLE flickering
    float currentIdlePWM;
    float targetIdlePWM;
    unsigned long lastIdleChange = 0;
    
    // Internal base recipe values specific to this historical location
    int baseBaseline, baseIdleIntensity, baseChaosAmplitude;
    float baseHarmonicDamping;
    
    // "Personality" variables to make each LED unique, preventing synchronized blinking
    int brightnessOffset; 
    float flickerSpeedMod;
    float chaosMod;

    // --- INITIALIZATION ---
    void init(int pin, Location loc) {
      ledPin = pin;
      locationType = loc;

      // Attach the pin to the ESP32 hardware PWM (ESP32 Core v3.x API)
      ledcAttach(ledPin, freq, resolution);
      
      // Generate unique physical traits for this specific lantern
      brightnessOffset = random(-200, 200);       // Slightly dimmer or brighter than its peers
      flickerSpeedMod = random(80, 120) / 100.0;  // 0.8x to 1.2x innate flicker speed
      chaosMod = random(80, 120) / 100.0;         // 0.8x to 1.2x innate chaotic reaction

      // Load the historical physics recipe based on the location
      switch (locationType) {
        case STERN:
          baseBaseline = 3500;         // Very bright to cut through fog
          baseIdleIntensity = 800;     // High flicker from exposed sea winds
          baseChaosAmplitude = 1500;   // Massive recovery swings
          baseHarmonicDamping = 0.010; // Low damping: a huge flame bounces around a lot
          break;
        case CABIN:
          baseBaseline = 3000;         // Bright, warm, elegant glow
          baseIdleIntensity = 100;     // Very steady, protected from drafts
          baseChaosAmplitude = 800;    // Moderate reaction to gusts
          baseHarmonicDamping = 0.040; // High damping: high quality wax settles very fast
          break;
        case BINNACLE:
          baseBaseline = 1000;         // Dim, just enough to read the compass
          baseIdleIntensity = 20;      // Barely flickers, heavily enclosed in brass
          baseChaosAmplitude = 200;    // Wind barely penetrates the housing
          baseHarmonicDamping = 0.050; // Max damping: tiny, stiff flame snaps right back
          break;
        case BATTLE:
          baseBaseline = 2000;         // Medium brightness below deck
          baseIdleIntensity = 400;     // Drafts whistling through open gun ports
          baseChaosAmplitude = 1000;   // Wild swings when cannons fire / wind gusts
          baseHarmonicDamping = 0.025; // Medium settling speed
          break;
        case KITCHEN:
          baseBaseline = 4000;         // Maximum brightness
          baseIdleIntensity = 1500;    // A roaring fire is never still
          baseChaosAmplitude = 2000;   // Chaotic and violent
          baseHarmonicDamping = 0.001; // Minimum damping: a wood fire never truly "settles"
          break;
      }
      
      // Start the flame at its normal brightness
      currentIdlePWM = baseBaseline;
      targetIdlePWM = baseBaseline;
    }

    // --- PHYSICS UPDATE LOOP ---
    void update(unsigned long currentTime, float bScale, float iScale, float cScale, float hScale, float sScale) {
      
      // Calculate this LED's exact parameters by combining:
      // 1. The Base Recipe  2. The Global Potentiometer Scales  3. The Unique Personality
      int myBasePWM = constrain((baseBaseline * bScale) + brightnessOffset, 50, 4095);
      int myIdleInt = baseIdleIntensity * iScale * chaosMod;
      int myChaosAmp = baseChaosAmplitude * cScale * chaosMod;
      float myHarmDamp = baseHarmonicDamping * hScale;
      
      unsigned long timeInState = currentTime - stateStartTime;
      int outputPWM = myBasePWM;

      // Adjust state durations based on the Global Speed pot (sScale)
      // Faster speed = shorter durations
      int dropDuration = 150 / sScale;
      int chaosDuration = 600 / sScale;
      int harmonicDuration = 2000 / sScale;

      // Execute the math for the current state of the flame
      switch (currentState) {
        
        case IDLE:
          // Is it time to pick a new target brightness? (Shorter intervals if sScale is high)
          if (currentTime - lastIdleChange > (random(20, 80) * flickerSpeedMod / sScale)) {
            targetIdlePWM = myBasePWM + random(-myIdleInt, myIdleInt);
            lastIdleChange = currentTime;
          }
          // Smoothly glide the current brightness toward the target brightness
          // The 0.2 multiplier defines the "gliding" speed. We scale it by sScale so it stays smooth at all tempos.
          currentIdlePWM += (targetIdlePWM - currentIdlePWM) * 0.2 * sScale;
          outputPWM = currentIdlePWM;
          break;

        case DROP:
          // Phase 1 of a gust: The flame is nearly blown out, recovering linearly back to baseline
          if (timeInState < dropDuration) {
            outputPWM = map(timeInState, 0, dropDuration, 100, myBasePWM);
          } else {
            currentState = CHAOTIC; // Move to next phase
            stateStartTime = currentTime;
          }
          break;

        case CHAOTIC:
          // Phase 2 of a gust: Violent sputtering
          if (timeInState < chaosDuration) {
            // Create an 'envelope' that goes from 1.0 down to 0.0 as time passes
            float envelope = 1.0 - ((float)timeInState / chaosDuration);
            int currentChaos = myChaosAmp * envelope;
            
            // Prevent random(0) which causes errors, ensuring a minimum of 1
            if (currentChaos < 1) currentChaos = 1; 
            
            outputPWM = myBasePWM + random(-currentChaos, currentChaos);
          } else {
            currentState = HARMONIC; // Move to final phase
            stateStartTime = currentTime;
          }
          break;

        case HARMONIC:
          // Phase 3 of a gust: The flame "bounces" as it re-establishes thermal equilibrium
          if (timeInState < harmonicDuration) {
            float timeSec = timeInState / 1000.0;
            
            // Calculate ringing frequency. Speed pot increases how fast it vibrates.
            float frequency = 8.0 * flickerSpeedMod * sScale; 
            float oscillation = sin(2 * PI * frequency * timeSec);
            
            // Calculate how fast the bouncing fades away
            float damping = exp(-myHarmDamp * timeInState * sScale); 
            
            outputPWM = myBasePWM + (int)(myChaosAmp * 0.5 * damping * oscillation);

            // If the bouncing is basically undetectable (under 5%), cleanly exit back to IDLE
            if (damping < 0.05) {
              currentState = IDLE;
              currentIdlePWM = outputPWM; 
            }
          } else {
            // Safety catch to force return to IDLE if time runs out
            currentState = IDLE;
          }
        break;
      }

      // Constrain to 12-bit bounds to prevent integer overflows causing weird flashes
      outputPWM = constrain(outputPWM, 0, 4095);

      // --- GAMMA CORRECTION ---
      // Human eyes perceive brightness logarithmically, not linearly. 
      // Squaring the normalized value maps the math physically correctly to the eye.
      float normalized = outputPWM / 4095.0;
      ledcWrite(ledPin, (int)((normalized * normalized) * 4095));
    }

    // Externally triggers the wind gust effect
    void triggerDrop(unsigned long time) {
      if (currentState == IDLE) {
        currentState = DROP;
        stateStartTime = time;
        }
    }
};

// ==============================================================================
// MAIN PROGRAM EXECUTION
// ==============================================================================

Flame flames[NUM_FLAMES];

// --- WAVE SEQUENCER VARIABLES ---
bool waveActive = false;
unsigned long waveStartTime = 0;
int currentWaveIndex = 0;
int battleLanternsTriggered = 0;

void setup() {
  Serial.begin(115200);
  while(!Serial);

  pinMode(triggerPin, INPUT); // the trigger button

  // Seed the random generator by reading environmental noise on an unconnected pin
  randomSeed(analogRead(36)); 

  // Initialize your 14 LEDs and assign them to specific locations on your model
  flames[0].init(ledPins[0], STERN);     // 1 Stern Lantern
  flames[1].init(ledPins[1], CABIN);     // 1 Great Cabin
  flames[2].init(ledPins[2], BINNACLE);  // 1 Binnacle
  flames[3].init(ledPins[3], KITCHEN);   // 3 Galley Fire LEDs
  flames[4].init(ledPins[4], KITCHEN);
  flames[5].init(ledPins[5], KITCHEN);
  
  // 8 Battle Lanterns (These are the ones that react to the "Wind Wave")
  flames[6].init(ledPins[6], BATTLE);    
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
  
  // --- READ POTENTIOMETERS ---
  // The Speed pot has a tighter range (0.7x to 1.5x) so it feels like changing the "weather" 
  // without breaking the physical realism of the simulation.
  float masterSpeedScale =    map(analogRead(potSpeedPin), 0, 4095, 70, 150) / 100.0;
  
  // The rest map to a 0.0 to 2.0 multiplier scale (0% to 200%)
  float masterBaselineScale = map(analogRead(potBaselinePin), 0, 4095, 0, 200) / 100.0;
  float masterIdleScale =     map(analogRead(potIdlePin),     0, 4095, 0, 200) / 100.0;
  float masterChaosScale =    map(analogRead(potChaosPin),    0, 4095, 0, 200) / 100.0;
  float masterHarmonicScale = map(analogRead(potHarmonicPin), 0, 4095, 0, 200) / 100.0;

  Serial.println("masterSpeedScale = " + String(masterSpeedScale));
  //Serial.println("masterBaselineScale = " + String(masterBaselineScale));
  //Serial.println("masterIdleScale = " + String(masterIdleScale));
  //Serial.println("masterChaosScale = " + String(masterChaosScale));
  //Serial.println("masterHarmonicScale = " + String(masterHarmonicScale));

  // --- TRIGGER THE WAVE ---
  // When button goes HIGH (pressed), start the wind draft
  if (digitalRead(triggerPin) == HIGH && !waveActive) {
    waveActive = true;
    waveStartTime = currentTime; 
    currentWaveIndex = 0; 
    battleLanternsTriggered = 0;  
  }

  // --- PROCESS THE WAVE ---
  if (waveActive) {
    if (currentWaveIndex < NUM_FLAMES) {
      
      // Only the Battle Lanterns get hit by the wind wave
      if (flames[currentWaveIndex].locationType != BATTLE) {
        currentWaveIndex++;
      } else {
        
        // The higher the Speed Pot is, the faster the draft travels down the deck!
        int currentWaveDelay = 80 / masterSpeedScale;

        if (currentTime > waveStartTime + (battleLanternsTriggered * currentWaveDelay)) {
          flames[currentWaveIndex].triggerDrop(currentTime);
          currentWaveIndex++;
          battleLanternsTriggered++;
        }
      }
    } else {
      // Wave is over
      waveActive = false;
      }
  }

  // --- UPDATE ALL PHYSICS ---
  for (int i = 0; i < NUM_FLAMES; i++) {
    flames[i].update(currentTime, masterBaselineScale, masterIdleScale, masterChaosScale, masterHarmonicScale, masterSpeedScale);
  }
  
  // The loop delay pacing!
  // This ensures the math runs at ~100Hz (like your original photodiode data). 
  // Without this, the ESP32 loops infinitely fast, which breaks the gliding math.
  delay(10); 
}