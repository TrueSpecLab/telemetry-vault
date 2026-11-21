/*
  TrueSpec Lab: Teensy 4.0 Sub-10mA Hardware STOP Mode (Zero-Pulse)
  - Disables ARM SysTick during sleep so CPU stays halted
  - Powers down USBPHY1 transceivers during STOP (avoids 18mA USB pulses & re-enumeration)[cite: 1]
  - Resets GPT2 counter cleanly with GPT_CR_ENMOD on 32.768kHz crystal
  - Correctly saves and restores DCDC and USBPHY registers on wake
*/

#include <imxrt.h>

#define WAKEUP_PIN     2     // Pin 2 (GPIO6_IO02 on Teensy 4.0)
#define SLEEP_SECONDS  10

unsigned long cycleCount = 0;
volatile bool timerExpired = false;
volatile bool pinTriggered = false;

void wakeISR() {
  pinTriggered = true;
}

void gpt2_isr() {
  GPT2_SR = 0x3F;
  timerExpired = true;
  asm volatile("dsb");
}

void enterLowPowerStop(uint32_t seconds) {
  timerExpired = false;
  pinTriggered = false;

  // 1. Disable ARM Cortex-M7 SysTick timer
  uint32_t saved_systick = SYST_CSR;
  SYST_CSR &= ~(SYST_CSR_TICKINT | SYST_CSR_ENABLE);

  // 2. Save all 5 GPC Interrupt Mask Registers and mask everything
  uint32_t saved_imr1 = GPC_IMR1;
  uint32_t saved_imr2 = GPC_IMR2;
  uint32_t saved_imr3 = GPC_IMR3;
  uint32_t saved_imr4 = GPC_IMR4;
  uint32_t saved_imr5 = GPC_IMR5;

  GPC_IMR1 = 0xFFFFFFFF;
  GPC_IMR2 = 0xFFFFFFFF;
  GPC_IMR3 = 0xFFFFFFFF;
  GPC_IMR4 = 0xFFFFFFFF;
  GPC_IMR5 = 0xFFFFFFFF;

  // 3. Unmask ONLY GPT2 (IRQ 101) and GPIO6_7_8_9 combined interrupt (IRQ 157)
  GPC_IMR4 &= ~(1 << (IRQ_GPT2 - 96));
  GPC_IMR5 &= ~(1 << (IRQ_GPIO6789 - 128));

  // 4. Configure GPT2 on 32.768 kHz crystal with ENMOD
  CCM_CCGR0 |= CCM_CCGR0_GPT2_BUS(CCM_CCGR_ON) | CCM_CCGR0_GPT2_SERIAL(CCM_CCGR_ON);
  GPT2_CR = 0;
  GPT2_CR = GPT_CR_SWR;
  while (GPT2_CR & GPT_CR_SWR);

  GPT2_PR = 0;
  GPT2_OCR1 = seconds * 32768UL;
  GPT2_SR = 0x3F;
  GPT2_IR = GPT_IR_OF1IE;
  GPT2_CR = GPT_CR_EN | GPT_CR_ENMOD | GPT_CR_CLKSRC(4) | GPT_CR_STOPEN;

  NVIC_CLEAR_PENDING(IRQ_GPT2);
  NVIC_ENABLE_IRQ(IRQ_GPT2);

  // 5. Save and configure CCM for TRUE STOP mode
  uint32_t saved_clpcr = CCM_CLPCR;
  CCM_CLPCR = (CCM_CLPCR & ~CCM_CLPCR_LPM(3))
              | CCM_CLPCR_LPM(2)
              | CCM_CLPCR_ARM_CLK_DIS_ON_LPM
              | CCM_CLPCR_SBYOS
              | CCM_CLPCR_COSC_PWRDOWN
              | CCM_CLPCR_BYPASS_LPM_HS0
              | CCM_CLPCR_BYPASS_LPM_HS1;

  // Save and power down USBPHY1 transceivers to eliminate USB line power drain
  uint32_t saved_usbphy1_pwd = USBPHY1_PWD;
  USBPHY1_PWD = 0x001E1C00;

  // Save and switch internal regulators / DCDC to low-power retention mode
  uint32_t saved_dcdc = DCDC_REG3;
  PMU_REG_2P5_SET = PMU_REG_2P5_ENABLE_WEAK_LINREG;
  PMU_REG_1P1_SET = PMU_REG_1P1_ENABLE_WEAK_LINREG;
  DCDC_REG3 = DCDC_REG0_PWD_CMP_OFFSET | DCDC_REG0_PWD_ZCD;

  SCB_SCR |= SCB_SCR_SLEEPDEEP;

  // 6. Halt CPU in STOP mode until GPT2 or Pin 2 fires
  while (!timerExpired && !pinTriggered && digitalRead(WAKEUP_PIN) == HIGH) {
    asm volatile("dsb");
    asm volatile("wfi");
  }

  // 7. Restore RUN mode, DCDC/PMU regulators, USBPHY, GPC masks, and SysTick
  SCB_SCR &= ~SCB_SCR_SLEEPDEEP;
  CCM_CLPCR = saved_clpcr;

  DCDC_REG3 = saved_dcdc;
  USBPHY1_PWD = saved_usbphy1_pwd;

  PMU_REG_2P5_CLR = PMU_REG_2P5_ENABLE_WEAK_LINREG;
  PMU_REG_1P1_CLR = PMU_REG_1P1_ENABLE_WEAK_LINREG;

  GPT2_CR = 0;
  NVIC_DISABLE_IRQ(IRQ_GPT2);

  GPC_IMR1 = saved_imr1;
  GPC_IMR2 = saved_imr2;
  GPC_IMR3 = saved_imr3;
  GPC_IMR4 = saved_imr4;
  GPC_IMR5 = saved_imr5;

  SYST_CSR = saved_systick;
}

void setup() {
  for (int i = 0; i < CORE_NUM_DIGITAL; i++) {
    if (i != WAKEUP_PIN) {
      pinMode(i, INPUT_DISABLE);
    }
  }

  pinMode(WAKEUP_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(WAKEUP_PIN), wakeISR, FALLING);
  attachInterruptVector(IRQ_GPT2, gpt2_isr);

  Serial.begin(115200);
  delay(3000); // 3-second boot window for clean USB auto-uploads
  Serial.println("\n--- TrueSpec Lab: Teensy 4.0 Zero-Pulse STOP Mode ---");
}

void loop() {
  cycleCount++;
  Serial.println("\n--- Active Cycle #" + String(cycleCount) + " ---");

  // Active-Low Hold Logic
  if (digitalRead(WAKEUP_PIN) == LOW) {
    Serial.println("Input pin is LOW. Holding active state...");
    while (digitalRead(WAKEUP_PIN) == LOW) {
      delay(50);
    }
    Serial.println("Input pin went HIGH. Proceeding to sleep.");
  }

  Serial.println("Entering Zero-Pulse Hardware STOP mode...");
  Serial.flush();

  enterLowPowerStop(SLEEP_SECONDS);

  if (pinTriggered || digitalRead(WAKEUP_PIN) == LOW) {
    Serial.println("Woke up via Pin 2 (Active Low)!");
  } else {
    Serial.println("Woke up via 10s Timer!");
  }
}