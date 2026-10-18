// ==============================================================================
// 17TH CENTURY GALLEON FLAME SYNTHESIZER (SAMD51 DIRECT REGISTER PWM + TC4)
// ==============================================================================

// --- PIN DEFINITIONS ---
const int triggerPin = 13;          // Connect a button from this pin to GND

// Potentiometer pins (Using analog-capable pins A0 - A3)
const uint8_t potIdlePin = A0;      
const uint8_t potChaosPin = A1;     
const uint8_t potHarmonicPin = A2;  
const uint8_t potBaselinePin = A3;  

// --- MULTI-LED SETUP ---
const int NUM_FLAMES = 8;
const int ledPins[NUM_FLAMES] = {21, 22, 5, 6, 9, 10, 11, 12};

enum FlameState { IDLE, DROP, CHAOTIC, HARMONIC };
enum Location { STERN, CABIN, BINNACLE, BATTLE, KITCHEN };

// --- NATIVE HARDWARE TIMER SETUP (TC4 at 100Hz) ---
volatile bool timerFlag = false;

void setupTC4Timer() {
  GCLK->PCHCTRL[TC4_GCLK_ID].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK0;

  TC4->COUNT16.CTRLA.bit.ENABLE = 0;
  while (TC4->COUNT16.SYNCBUSY.bit.ENABLE);
  TC4->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
  while (TC4->COUNT16.SYNCBUSY.bit.SWRST);

  TC4->COUNT16.CTRLA.reg = TC_CTRLA_MODE_COUNT16 | TC_CTRLA_PRESCALER_DIV1024;
  TC4->COUNT16.WAVE.reg = TC_WAVE_WAVEGEN_MFRQ;
  TC4->COUNT16.CC[0].reg = 469; // 48MHz / 1024 / 100Hz = 100Hz tick
  while (TC4->COUNT16.SYNCBUSY.bit.CC0);

  TC4->COUNT16.INTENSET.reg = TC_INTENSET_MC0;

  NVIC_DisableIRQ(TC4_IRQn);
  NVIC_ClearPendingIRQ(TC4_IRQn);
  NVIC_SetPriority(TC4_IRQn, 0);
  NVIC_EnableIRQ(TC4_IRQn);

  TC4->COUNT16.CTRLA.bit.ENABLE = 1;
  while (TC4->COUNT16.SYNCBUSY.bit.ENABLE);
}

void TC4_Handler() {
  if (TC4->COUNT16.INTFLAG.reg & TC_INTFLAG_MC0) {
    TC4->COUNT16.INTFLAG.reg = TC_INTFLAG_MC0;
    timerFlag = true;
  }
}

// ==============================================================================
// THE FLAME CLASS
// ==============================================================================
class Flame {
  public:
    int ledPin;
    FlameState currentState = IDLE;
    Location locationType;
    unsigned long stateStartTime = 0;
    
    float currentIdlePWM;
    float targetIdlePWM;
    unsigned long lastIdleChange = 0;
    
    int baseBaseline, baseIdleIntensity, baseChaosAmplitude;
    float baseHarmonicDamping;
    
    int brightnessOffset; 
    float flickerSpeedMod;
    float chaosMod;
    int dropStartPWM = 0;

    void init(int pin, Location loc) {
      ledPin = pin;
      locationType = loc;

      pinMode(ledPin, OUTPUT);
      analogWrite(ledPin, 0); 

      brightnessOffset = random(-200, 200); 
      flickerSpeedMod = random(80, 120) / 100.0; 
      chaosMod = random(80, 120) / 100.0;         

      switch (locationType) {
        case STERN:    baseBaseline = 3500; baseIdleIntensity = 800;  baseChaosAmplitude = 1500; baseHarmonicDamping = 0.010; break;
        case CABIN:    baseBaseline = 3000; baseIdleIntensity = 100;  baseChaosAmplitude = 800;  baseHarmonicDamping = 0.040; break;
        case BINNACLE: baseBaseline = 1000; baseIdleIntensity = 20;   baseChaosAmplitude = 200;  baseHarmonicDamping = 0.050; break;
        case BATTLE:   baseBaseline = 2000; baseIdleIntensity = 400;  baseChaosAmplitude = 1000; baseHarmonicDamping = 0.025; break;
        case KITCHEN:  baseBaseline = 4000; baseIdleIntensity = 1500; baseChaosAmplitude = 2000; baseHarmonicDamping = 0.001; break;
      }
      currentIdlePWM = baseBaseline;
      targetIdlePWM = baseBaseline;
    }

    void update(unsigned long currentTime, float bScale, float iScale, float cScale, float hScale) {
      int myBasePWM = constrain((baseBaseline * bScale) + brightnessOffset, 50, 4095);
      int myIdleInt = baseIdleIntensity * iScale * chaosMod;
      int myChaosAmp = baseChaosAmplitude * cScale * chaosMod;
      float myHarmDamp = baseHarmonicDamping * hScale;
      
      unsigned long timeInState = currentTime - stateStartTime;
      int outputPWM = myBasePWM;

      switch (currentState) {
        case IDLE:
          if (currentTime - lastIdleChange > random(20 * flickerSpeedMod, 80 * flickerSpeedMod)) {
            targetIdlePWM = myBasePWM + random(-myIdleInt, myIdleInt);
            lastIdleChange = currentTime;
          }
          currentIdlePWM += (targetIdlePWM - currentIdlePWM) * 0.15;
          outputPWM = currentIdlePWM;
          break;

        case DROP:
          if (timeInState < 150) {
            float t = (float)timeInState / 150.0;
            int dipTarget = myBasePWM * 0.15; 
            outputPWM = dropStartPWM + (dipTarget - dropStartPWM) * t;
          } else {
            currentState = CHAOTIC;
            stateStartTime = currentTime;
            currentIdlePWM = outputPWM;
          }
          break;

        case CHAOTIC:
          if (timeInState < 600) {
            float envelope = 1.0 - ((float)timeInState / 600.0);
            outputPWM = myBasePWM + random(-myChaosAmp * envelope, myChaosAmp * envelope);
            currentIdlePWM = outputPWM;
          } else {
            currentState = HARMONIC;
            stateStartTime = currentTime;
          }
          break;

        case HARMONIC:
          if (timeInState < 2000) {
            float timeSec = timeInState / 1000.0;
            float frequency = 8.0 * flickerSpeedMod; 
            float oscillation = sin(2 * PI * frequency * timeSec);
            float damping = exp(-myHarmDamp * timeInState); 
            
            outputPWM = myBasePWM + (int)(myChaosAmp * 0.5 * damping * oscillation);
            currentIdlePWM = outputPWM;
            
            if (damping < 0.05) {
              currentState = IDLE;
              currentIdlePWM = outputPWM;
            }
          } else {
            currentState = IDLE;
            currentIdlePWM = outputPWM;
          }
          break;
      }

      outputPWM = constrain(outputPWM, 0, 4095);
      
      // Gamma correction curve
      float normalized = outputPWM / 4095.0;
      int gammaPWM = (int)((normalized * normalized) * 4095);
      
      // Direct pin update via Arduino's optimized wrapper (bypassing redundant checks)
      analogWrite(ledPin, gammaPWM);
    }

    void triggerDrop(unsigned long time, int currentPWM) {
      if (currentState == IDLE) { 
        currentState = DROP; 
        stateStartTime = time; 
        dropStartPWM = currentPWM; 
      }
    }
};

Flame flames[NUM_FLAMES];
bool waveActive = false;
unsigned long waveStartTime = 0;
int currentWaveIndex = 0;
int battleLanternsTriggered = 0;
const int waveDelayMS = 80; 

unsigned long lastPotReadMillis = 0;
const unsigned long potReadIntervalMillis = 100; 
float bScale = 1.0, iScale = 1.0, cScale = 1.0, hScale = 1.0;

void setup() {
  Serial.begin(115200);
  
  analogWriteResolution(12);
  analogReadResolution(12);

  pinMode(triggerPin, INPUT);
  randomSeed(analogRead(A5)); 

  flames[0].init(ledPins[0], STERN);     
  flames[1].init(ledPins[1], CABIN);     
  flames[2].init(ledPins[2], BINNACLE);  
  flames[3].init(ledPins[3], KITCHEN);   
  flames[4].init(ledPins[4], BATTLE);    
  flames[5].init(ledPins[5], BATTLE);
  flames[6].init(ledPins[6], BATTLE);
  flames[7].init(ledPins[7], BATTLE);

  setupTC4Timer();
}

void loop() {
  unsigned long currentTime = millis();
  
  if (currentTime - lastPotReadMillis >= potReadIntervalMillis) {
    lastPotReadMillis = currentTime;
    
    int rawBase = analogRead(potBaselinePin);
    int rawIdle = analogRead(potIdlePin);
    int rawChaos = analogRead(potChaosPin);
    int rawHarm = analogRead(potHarmonicPin);

    bScale = (rawBase > 50) ? (map(rawBase, 0, 4095, 0, 200) / 100.0) : 1.0;
    iScale = (rawIdle > 50) ? (map(rawIdle, 0, 4095, 0, 200) / 100.0) : 1.0;
    cScale = (rawChaos > 50) ? (map(rawChaos, 0, 4095, 0, 200) / 100.0) : 1.0;
    hScale = (rawHarm > 50) ? (map(rawHarm, 0, 4095, 0, 200) / 100.0) : 1.0;
  }

  if (digitalRead(triggerPin) == HIGH && !waveActive) {
    waveActive = true; 
    waveStartTime = currentTime; 
    currentWaveIndex = 0; 
    battleLanternsTriggered = 0;
  }

  if (waveActive) {
    if (currentWaveIndex < NUM_FLAMES) {
      if (flames[currentWaveIndex].locationType != BATTLE) {
        currentWaveIndex++;
      } else if (currentTime > waveStartTime + (battleLanternsTriggered * waveDelayMS)) {
        flames[currentWaveIndex].triggerDrop(currentTime, (int)flames[currentWaveIndex].currentIdlePWM);
        currentWaveIndex++;
        battleLanternsTriggered++;
      }
    } else { 
      waveActive = false; 
    }
  }

  if (timerFlag) {
    timerFlag = false; 
    
    for (int i = 0; i < NUM_FLAMES; i++) {
      flames[i].update(currentTime, bScale, iScale, cScale, hScale);
    }
  }
}