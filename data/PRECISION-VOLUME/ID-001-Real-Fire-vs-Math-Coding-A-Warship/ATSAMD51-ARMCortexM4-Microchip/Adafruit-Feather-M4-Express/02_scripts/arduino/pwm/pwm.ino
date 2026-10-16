// The 14 target lantern pins
const uint8_t lanternPins[14] = {21, 22, 5, 6, 9, 10, 11, 12, 13, 4, 1, 0, 23, 24};

struct LanternState {
  uint8_t pin;
  uint16_t currentDuty;
  uint16_t targetDuty;
  unsigned long lastStepTime;
  unsigned long stepInterval;
};

LanternState lanterns[14];

void setup() {
  // 1. Enable APB clocks for TCC0, TCC1, TC3, and TC4
  MCLK->APBBMASK.reg |= MCLK_APBBMASK_TCC0 | MCLK_APBBMASK_TCC1;
  MCLK->APBAMASK.reg |= MCLK_APBBMASK_TC3;
  MCLK->APBAMASK.reg |= MCLK_APBCMASK_TC4;

  // 2. Configure Master Generic Clock (GCLK7) at 16MHz (48MHz / 3)
  GCLK->GENCTRL[7].reg = GCLK_GENCTRL_DIV(1) |       
                         GCLK_GENCTRL_IDC |          
                         GCLK_GENCTRL_GENEN |        
                         GCLK_GENCTRL_SRC_DFLL;      
  while (GCLK->SYNCBUSY.bit.GENCTRL7);               

  // Route GCLK7 to peripheral channels for TCC0/1 (25) and TC3/4 (26, 30)
  GCLK->PCHCTRL[25].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK7;
  GCLK->PCHCTRL[26].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK7; // TC3
  GCLK->PCHCTRL[30].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK7; // TC4

  // 3. Configure TCC0 & TCC1 for 50Hz Base Period (PER = 19999 at 1MHz timer clock)
  TCC0->WAVE.reg = TCC_WAVE_WAVEGEN_NPWM;
  while (TCC0->SYNCBUSY.bit.WAVE);
  TCC0->CTRLA.reg = TCC_CTRLA_PRESCALER_DIV16;
  TCC0->PER.reg = 19999;
  while (TCC0->SYNCBUSY.bit.PER);
  TCC0->CTRLA.bit.ENABLE = 1;
  while (TCC0->SYNCBUSY.bit.ENABLE);

  TCC1->WAVE.reg = TCC_WAVE_WAVEGEN_NPWM;
  while (TCC1->SYNCBUSY.bit.WAVE); // wait if needed or WAVE sync
  TCC1->CTRLA.reg = TCC_CTRLA_PRESCALER_DIV16;
  TCC1->PER.reg = 19999;
  while (TCC1->SYNCBUSY.bit.PER);
  TCC1->CTRLA.bit.ENABLE = 1;
  while (TCC1->SYNCBUSY.bit.ENABLE);

  // 4. Initialize and map each pin to its correct hardware peripheral
  for (int i = 0; i < 14; i++) {
    uint8_t pin = lanternPins[i];
    uint32_t portPin = g_APinDescription[pin].ulPin;
    uint8_t portGroup = g_APinDescription[pin].ulPort;

    // Set pin direction to output
    PORT->Group[portGroup].DIRSET.reg = (1ul << portPin);
    PORT->Group[portGroup].PINCFG[portPin].bit.PMUXEN = 1;

    // Correct PMUX assignment per pin group (Peripheral F for TCC0/1, Peripheral E for TC3/4)
    uint8_t muxFunction = (i < 8) ? 5 : 4; // Distribute across TCC (5) and TC (4)
    if (portPin & 1) {
      PORT->Group[portGroup].PMUX[portPin >> 1].reg |= PORT_PMUX_PMUXO(muxFunction);
    } else {
      PORT->Group[portGroup].PMUX[portPin >> 1].reg |= PORT_PMUX_PMUXE(muxFunction);
    }

    // Initialize state
    lanterns[i].pin = pin;
    lanterns[i].currentDuty = 5000;
    lanterns[i].targetDuty = 5000;
    lanterns[i].lastStepTime = 0;
    lanterns[i].stepInterval = random(50, 160);
  }
}

// Write duty cycle to the appropriate hardware buffer register without clobbering
void setLanternBrightness(uint8_t index, float brightness) {
  if (brightness < 0.0f) brightness = 0.0f;
  if (brightness > 1.0f) brightness = 1.0f;

  // Gamma correction curve for natural human eye response
  uint16_t dutyTicks = (uint16_t)(19999.0f * (brightness * brightness));

  if (index < 6) {
    TCC0->CCBUF[index].reg = dutyTicks;
  } else if (index < 10) {
    TCC1->CCBUF[index - 6].reg = dutyTicks;
  } else {
    // Basic TC channels managed via direct register sync
    TC3->COUNT16.CC[0].reg = dutyTicks;
    while (TC3->COUNT16.SYNCBUSY.bit.CC0);
  }
}

void loop() {
  unsigned long currentMillis = millis();

  // Run independent non-blocking flicker updates for all 14 channels
  for (int i = 0; i < 14; i++) {
    if (currentMillis - lanterns[i].lastStepTime >= lanterns[i].stepInterval) {
      lanterns[i].lastStepTime = currentMillis;
      lanterns[i].stepInterval = random(40, 180);

      float currentNorm = sqrt((float)lanterns[i].currentDuty / 19999.0f);
      float targetNorm = random(15, 95) / 100.0f; // Ambient lantern glow range
      
      currentNorm += (targetNorm - currentNorm) * 0.35f;
      
      setLanternBrightness(i, currentNorm);
      lanterns[i].currentDuty = (uint16_t)(19999.0f * (currentNorm * currentNorm));
    }
  }
}