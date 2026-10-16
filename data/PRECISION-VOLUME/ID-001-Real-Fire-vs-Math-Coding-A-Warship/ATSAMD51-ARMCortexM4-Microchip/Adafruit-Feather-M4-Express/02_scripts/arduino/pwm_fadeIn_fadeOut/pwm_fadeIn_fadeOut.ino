void setup() {
  // Output PWM on pin 4 using TC3 (Channel 0 / WO[0]) in Normal PWM mode (~45.8 Hz)
  MCLK->APBAMASK.reg |= MCLK_APBDMASK_TC7;           // Activate timer TC3
  
  // Set up the generic clock (GCLK7) at 48MHz
  GCLK->GENCTRL[7].reg = GCLK_GENCTRL_DIV(1) |       
                         GCLK_GENCTRL_IDC |          
                         GCLK_GENCTRL_GENEN |        
                         GCLK_GENCTRL_SRC_DFLL;      
  while (GCLK->SYNCBUSY.bit.GENCTRL7);               

  // Connect GCLK7 to TC3 peripheral channel (TC3_GCLK_ID is 26)
  GCLK->PCHCTRL[TC5_GCLK_ID].reg = GCLK_PCHCTRL_CHEN | 
                                    GCLK_PCHCTRL_GEN_GCLK7;

  // Set pin 4 as an output in the port direction register
  PORT->Group[g_APinDescription[24].ulPort].DIRSET.reg = (1ul << g_APinDescription[24].ulPin);

  // Enable the peripheral multiplexer on pin 4
  PORT->Group[g_APinDescription[24].ulPort].PINCFG[g_APinDescription[24].ulPin].bit.PMUXEN = 1;
  
  // Configure PMUX for pin 4 to Peripheral E (value 4): TC3, Channel 0 (WO[0])
  uint32_t pin = g_APinDescription[24].ulPin;
  if (pin & 1) {
    PORT->Group[g_APinDescription[24].ulPort].PMUX[pin >> 1].reg |= PORT_PMUX_PMUXO(24);
  } else {
    PORT->Group[g_APinDescription[24].ulPort].PMUX[pin >> 1].reg |= PORT_PMUX_PMUXE(24);
  }
  
  // FIX: Use DIV16 to push PWM carrier frequency to ~45.8 Hz (eliminates blinking)
  TC7->COUNT16.CTRLA.reg = TC_CTRLA_PRESCALER_DIV16 |        
                           TC_CTRLA_PRESCSYNC_PRESC |        
                           TC_CTRLA_MODE_COUNT16;            

  // Use Normal PWM mode (NPWM) where CC[0] controls WO[0] duty cycle
  TC3->COUNT16.WAVE.reg = TC_WAVE_WAVEGEN_NPWM;      

  TC3->COUNT16.CC[0].reg = 32767;                    // Initial 50% duty cycle
  while (TC3->COUNT16.SYNCBUSY.bit.CC0);             

  TC3->COUNT16.CTRLA.bit.ENABLE = 1;                 
  while (TC3->COUNT16.SYNCBUSY.bit.ENABLE);          
}

void loop() {
  // Smooth exponential breathing loop targeting CC[0] directly on WO[0]
  for (int i = 0; i <= 255; i++) {
    float normalized = (float)i / 255.0;
    uint16_t duty = (uint16_t)(65535.0 * (normalized * normalized)); // Gamma correction
    
    TC3->COUNT16.CC[0].reg = duty;
    while (TC3->COUNT16.SYNCBUSY.bit.CC0);           // Ensure clock domain synchronization
    delay(10);
  }
  
  for (int i = 255; i >= 0; i--) {
    float normalized = (float)i / 255.0;
    uint16_t duty = (uint16_t)(65535.0 * (normalized * normalized));
    
    TC3->COUNT16.CC[0].reg = duty;
    while (TC3->COUNT16.SYNCBUSY.bit.CC0);
    delay(10);
  }
}