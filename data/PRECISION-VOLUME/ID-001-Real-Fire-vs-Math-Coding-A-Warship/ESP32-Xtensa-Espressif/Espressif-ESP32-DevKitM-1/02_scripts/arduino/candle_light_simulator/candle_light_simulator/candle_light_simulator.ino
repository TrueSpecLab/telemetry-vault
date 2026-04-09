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

// setting PWM properties
const int freq = 5000;
const int resolution = 12;

uint16_t brightnessMax = 4095;

// Suggested range: 50–70%
uint16_t brightnessBase = 0.58 * 4095;  // ~2457

// Minimum dip level: 45–55% of base
// Higher resolution allows sharper transitions
uint16_t brightnessMin = 0.5 * brightnessBase; // ~1200

float noise = 200;
float alpha = 0.2; // smoothing factor

// Smoother noise (reduce harsh jumps)
//float amplitude = 0.12 * brightnessMax;  // sweet spot (~12%); 8–15% of full scale
float chaoticAmp = 0.09 * brightnessMax;  // ~360
float harmonicAmp = 0.1 * brightnessMax;  // ≈ 410

float t = 0;

uint16_t dropDuration = 350; // ms value
uint16_t chaoticDuration = 1500; // ms value
uint16_t harmonicDuration = 3000; // ms value

void setup() {
  // Set output pins
  for(uint8_t i = 0; i < 14; i++){
    // Set pin as OUTPUT
    pinMode(gpio_pins[i], OUTPUT);

    // Drive OUTPUT LOW
    digitalWrite(gpio_pins[i], LOW);

    // configure LED PWM
    ledcAttach(gpio_pins[i], freq, resolution);
  }

  Serial.begin(115200);

  // if you want to attach a specific channel, use the following instead
  //ledcAttachChannel(ledPin, freq, resolution, 0);
}

void loop(){
  // Reconstruct the Flame Behavior
  // Phase 1: Initial Drop
  float brightnessPhase1 = brightnessBase;

  for (uint32_t tStart = millis(); (millis()-tStart) < dropDuration;) {
      brightnessPhase1 *= 0.9;  // exponential drop
      ledcWrite(gpio_pins[0], brightnessPhase1);
      delay(5);
  }
  // Phase 2: Chaotic Flicker - Use filtered noise:
  float brightnessPhase2 = brightnessPhase1;

  for (uint32_t tStart = millis(); (millis()-tStart) < chaoticDuration;) {
    float raw = random(0, 100) / 100.0;
    //noise = alpha * raw + (1 - alpha) * noise;
    noise = noise * (noise > 0 ? 1.4 : 0.6);

    brightnessPhase2 = brightnessBase + noise * chaoticAmp;
    ledcWrite(gpio_pins[0], constrain(brightnessPhase2, brightnessMin, brightnessMax));
    Serial.println(brightnessPhase2);
    delay(10);
  }

  // Phase 3: Harmonic Oscillation
  float brightnessPhase3 = brightnessPhase2;

  /*for (uint32_t tStart = millis(); (millis()-tStart) < harmonicDuration;) {
    float envelope = exp(-0.01 * t);
    float value = sin(2 * PI * freq * t) * envelope;

    float brightness = brightnessBase + value * harmonicAmp;
    ledcWrite(gpio_pins[0], constrain(brightness, brightnessMin, brightnessMax));

    t += 0.01;
    delay(10);
  }*/

  ledcWrite(gpio_pins[0], brightnessBase);
  delay(1000);
}