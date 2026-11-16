/*
  TrueSpecLab: ATSAMD51 Bare-Metal Standby Mode
  Targets: ItsyBitsy M4 Express, Feather M4 Express
*/

void setup() {
  Serial.begin(115200);
  while(!Serial) { delay(10); }

  Serial.println("Preparing for ARM Cortex-M4 Standby Mode...");
  
  // If using Feather M4, turn off the NeoPixel power to prevent parasitic drain
  pinMode(8, OUTPUT); 
  digitalWrite(8, LOW);

  Serial.println("Going to sleep. You will need to reset the board to wake it.");
  Serial.flush();

  // 1. Set up the SAMD51 to go into STANDBY mode during sleep
  PM->SLEEPCFG.reg = PM_SLEEPCFG_SLEEPMODE_STANDBY; 
  
  // 2. Wait for the STANDBY bitfield to be successfully set
  while(PM->SLEEPCFG.bit.SLEEPMODE != PM_SLEEPCFG_SLEEPMODE_STANDBY_Val); 

  // 3. Complete any outstanding memory operations
  __DSB(); 

  // 4. Put the SAMD51 into deep sleep (Wait For Interrupt)
  __WFI(); 
}

void loop() {
  // The CPU is halted. It will not reach this point unless an interrupt fires.
}
