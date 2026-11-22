/*
  TrueSpec Lab - Teensy 4.0 Low-Power Wake Diagnostic

  Version 2.1: removes unused GPC-mask warnings in CPU-only WFI builds.
  Starting configuration: CPU-only WFI + Pin 2 wake (isolation test).
  This is a diagnostic sketch, NOT a validated low-power production design.

  Physical connections (common ground required):
    Teensy 2       : wake input, active LOW (regular GPIO4_IO04)
    Teensy 3       : PPK2 D0, HIGH from low-power entry to full return
    Teensy 2       : optionally also PPK2 D1 (observe actual input voltage)
    Teensy 4       : PPK2 D4, HIGH once GPIO4 ISR actually runs
    Teensy 5       : PPK2 D2, 150 ms HIGH after pin wake
    Teensy 6       : PPK2 D3, 150 ms HIGH after GPT2 wake
    Onboard LED 13 : heartbeat while executing the application

  Changing configurations:
    POWER_MODE = POWER_MODE_CPU_WFI, POWER_MODE_WAIT, or POWER_MODE_STOP
    WAKE_MODE  = WAKE_BY_PIN, WAKE_BY_GPT2, or WAKE_BY_EITHER

  IMPORTANT:
    CPU_WFI checks repeated IRQ delivery without CCM WAIT/STOP transitions.
    WAIT/STOP are experimental and may fail because of power-down sequence
    details/RT1060 errata. Begin with CPU_WFI + PIN to isolate the issue.
    No blanket CCM_CCGR gating, USB PHY power-down, or PMU edits are used.
*/

#include <imxrt.h>

// ============================================================================
// 1. USER SETTINGS - change only these for the first series of tests
// ============================================================================

#define POWER_MODE_CPU_WFI 0  // Clock tree unchanged; test basic GPIO re-entry
#define POWER_MODE_WAIT    1  // CCM WAIT, known intermittent in this experiment
#define POWER_MODE_STOP    2  // Experimental STOP
#define POWER_MODE         POWER_MODE_WAIT

#define WAKE_BY_PIN       0
#define WAKE_BY_GPT2      1
#define WAKE_BY_EITHER    2
#define WAKE_MODE         WAKE_BY_EITHER

#define SLEEP_SECONDS              10U  // Used for GPT2 or EITHER only
#define ACTIVE_BEFORE_SLEEP_MS   5000U
#define LED_BLINK_INTERVAL_MS     500U

// ============================================================================
// 2. PIN DEFINITIONS / BUILD OPTIONS
// ============================================================================

#define WAKEUP_PIN        2
#define MARKER_PIN        3   // PPK2 D0: sleep routine active
#define IRQ_SEEN_PIN      4   // PPK2 D4: latched HIGH when GPIO4 ISR executes
// PPK2 D1 is reserved to measure Teensy pin 2 directly (not a GPIO output).
#define PIN_WAKE_DIAG_PIN 5   // PPK2 D2: pin was wake source
#define GPT_WAKE_DIAG_PIN 6   // PPK2 D3: GPT2 was wake source
#define LED_PIN           LED_BUILTIN

#define WAKE_MASK         (1u << 4) // Pin 2: GPIO_EMC_04 / GPIO4_IO04

#if (POWER_MODE != POWER_MODE_CPU_WFI) && (POWER_MODE != POWER_MODE_WAIT) && (POWER_MODE != POWER_MODE_STOP)
  #error "Invalid POWER_MODE"
#endif

#if (WAKE_MODE != WAKE_BY_PIN) && (WAKE_MODE != WAKE_BY_GPT2) && (WAKE_MODE != WAKE_BY_EITHER)
  #error "Invalid WAKE_MODE"
#endif

#if (WAKE_MODE == WAKE_BY_PIN) || (WAKE_MODE == WAKE_BY_EITHER)
  #define USE_PIN_WAKE 1
#else
  #define USE_PIN_WAKE 0
#endif

#if (WAKE_MODE == WAKE_BY_GPT2) || (WAKE_MODE == WAKE_BY_EITHER)
  #define USE_GPT2_WAKE 1
#else
  #define USE_GPT2_WAKE 0
#endif

// ============================================================================
// 3. APPLICATION STATE (maintained across a successful sleep/wake)
// ============================================================================

volatile bool pinTriggered = false;
volatile bool timerExpired = false;

bool ledState = false;
uint32_t lastLedToggleMs = 0;
uint32_t lastSleepCycleMs = 0;
uint32_t wakeCount = 0;
uint32_t cpuTaskCount = 0;

// ============================================================================
// 4. WAKE INPUT ON *REGULAR* GPIO4 INSTEAD OF TEENSY'S FAST GPIO9
// ============================================================================

#if USE_PIN_WAKE
static inline bool wakePinIsLow() {
  // digitalReadFast(2) would read GPIO9, which we have remapped away.
  return (GPIO4_PSR & WAKE_MASK) == 0;
}

void gpio4WakeISR() {
  // This sketch owns the GPIO4 0-15 IRQ vector; no other GPIO4 0-15 IRQ
  // users should be installed alongside it.
  GPIO4_ISR = WAKE_MASK;       // Write-one-to-clear
  pinTriggered = true;
  // Independent marker: even if the sleep routine never completes,
  // D4 will stay HIGH if this ISR actually executed.
  digitalWriteFast(IRQ_SEEN_PIN, HIGH);
  asm volatile("dsb" ::: "memory");
}

void configureLowPowerWakePin() {
  // Set the input pad & internal pull-up using normal Teensy APIs, BEFORE
  // remapping from fast GPIO9 to regular GPIO4.
  pinMode(WAKEUP_PIN, INPUT_PULLUP);
  CORE_PIN2_PADCONFIG |= IOMUXC_PAD_HYS;

  CCM_CCGR3 |= CCM_CCGR3_GPIO4(CCM_CCGR_ON);
  CCM_CCGR4 |= CCM_CCGR4_IOMUXC_GPR(CCM_CCGR_ON);

  // GPIO9 and GPIO4 share this pad; choose regular GPIO4, bit 4.
  GPIO9_IMR &= ~WAKE_MASK;
  IOMUXC_GPR_GPR29 &= ~WAKE_MASK;

  GPIO4_GDIR &= ~WAKE_MASK;      // input
  GPIO4_IMR &= ~0xFFFFu;        // own this 0-15 IRQ vector in this test
  GPIO4_EDGE_SEL &= ~WAKE_MASK; // use ICR1, not either-edge mode

  // GPIO4_IO04: ICR1 bits 9:8 = 0b11 (falling edge)
  GPIO4_ICR1 = (GPIO4_ICR1 & ~(3u << 8)) | (3u << 8);
  GPIO4_ISR = WAKE_MASK;        // clear old status

  attachInterruptVector(IRQ_GPIO4_0_15, gpio4WakeISR);
  NVIC_CLEAR_PENDING(IRQ_GPIO4_0_15);
  GPIO4_IMR |= WAKE_MASK;
  NVIC_ENABLE_IRQ(IRQ_GPIO4_0_15);
}
#endif

// ============================================================================
// 5. GPT2 (only compiled in timed / either modes)
// ============================================================================

#if USE_GPT2_WAKE
void gpt2_isr() {
  GPT2_SR = GPT_SR_OF1;
  timerExpired = true;
  asm volatile("dsb" ::: "memory");
}

uint32_t startGPT2Wakeup(uint32_t seconds) {
  uint32_t oldCCGR0 = CCM_CCGR0;
  CCM_CCGR0 |= CCM_CCGR0_GPT2_BUS(CCM_CCGR_ON) |
               CCM_CCGR0_GPT2_SERIAL(CCM_CCGR_ON);

  GPT2_CR = 0;
  GPT2_IR = 0;
  GPT2_CR = GPT_CR_SWR;
  while (GPT2_CR & GPT_CR_SWR) { }

  GPT2_PR = 0;
  GPT2_OCR1 = seconds * 32768UL;
  GPT2_SR = 0x3F;

  NVIC_CLEAR_PENDING(IRQ_GPT2);
  NVIC_ENABLE_IRQ(IRQ_GPT2);
  GPT2_IR = GPT_IR_OF1IE;

  // WAITEN is essential for GPT2 to continue in WAIT mode.
  // STOPEN is necessary for STOP mode; both may be set safely here.
  GPT2_CR = GPT_CR_ENMOD | GPT_CR_CLKSRC(4) |
            GPT_CR_WAITEN | GPT_CR_STOPEN | GPT_CR_EN;

  return oldCCGR0;
}

void stopGPT2Wakeup(uint32_t oldCCGR0) {
  GPT2_IR = 0;
  GPT2_CR = 0;
  GPT2_SR = 0x3F;
  NVIC_DISABLE_IRQ(IRQ_GPT2);
  NVIC_CLEAR_PENDING(IRQ_GPT2);
  CCM_CCGR0 = oldCCGR0;
}
#endif

static inline bool wakeConditionMet() {
#if USE_PIN_WAKE
  if (pinTriggered || wakePinIsLow()) return true;
#endif
#if USE_GPT2_WAKE
  if (timerExpired) return true;
#endif
  return false;
}

// ============================================================================
// 6. ENTER WAIT / STOP, THEN RESTORE THE NORMAL EXECUTION CONTEXT
// ============================================================================

void enterLowPower(uint32_t seconds) {
  // Enter only from loop() with interrupts initially enabled.
  noInterrupts();

#if USE_PIN_WAKE
  // If already held low, remain awake; no falling edge can be expected.
  if (wakePinIsLow()) {
    interrupts();
    return;
  }

#endif

  // Flags are cleared ONCE. Do not clear them again just before WFI.
  pinTriggered = false;
  timerExpired = false;

#if USE_PIN_WAKE
  // Re-arm and verify GPIO4 IRQ at EVERY sleep entry, not just setup.
  // Clear the latched ISR marker only after deciding to sleep.
  digitalWriteFast(IRQ_SEEN_PIN, LOW);
  GPIO4_IMR &= ~WAKE_MASK;
  GPIO4_ISR = WAKE_MASK;
  NVIC_CLEAR_PENDING(IRQ_GPIO4_0_15);
  GPIO4_IMR |= WAKE_MASK;
  NVIC_ENABLE_IRQ(IRQ_GPIO4_0_15);
#endif

  // SysTick would otherwise wake the CPU periodically.
  uint32_t savedSysTick = SYST_CSR;
  SYST_CSR &= ~(SYST_CSR_TICKINT | SYST_CSR_ENABLE);

  // GPC masks are changed only in WAIT or STOP modes.
  // Do not declare unused saved registers in CPU-only WFI builds.
#if POWER_MODE != POWER_MODE_CPU_WFI
  uint32_t savedIMR1 = GPC_IMR1;
  uint32_t savedIMR2 = GPC_IMR2;
  uint32_t savedIMR3 = GPC_IMR3;
  uint32_t savedIMR4 = GPC_IMR4;
  uint32_t savedIMR5 = GPC_IMR5;

  GPC_IMR1 = 0xFFFFFFFFu;
  GPC_IMR2 = 0xFFFFFFFFu;
  GPC_IMR3 = 0xFFFFFFFFu;
  GPC_IMR4 = 0xFFFFFFFFu;
  GPC_IMR5 = 0xFFFFFFFFu;
#endif

#if USE_PIN_WAKE && POWER_MODE != POWER_MODE_CPU_WFI
  GPC_IMR3 &= ~(1u << (IRQ_GPIO4_0_15 - 64));
#endif
#if USE_GPT2_WAKE
  #if POWER_MODE != POWER_MODE_CPU_WFI
  GPC_IMR4 &= ~(1u << (IRQ_GPT2 - 96));
  #endif
  uint32_t savedCCGR0 = startGPT2Wakeup(seconds);
#endif

  uint32_t savedCLPCR = CCM_CLPCR;
  uint32_t savedCGPR = CCM_CGPR;
  uint32_t savedSCR = SCB_SCR;

#if POWER_MODE == POWER_MODE_CPU_WFI
  // Isolation test: use WFI with no CCM WAIT/STOP transition and no GPC edits.
  // This tests the GPIO4 interrupt and software re-entry independently.
  CCM_CLPCR = (savedCLPCR & ~CCM_CLPCR_LPM(3)) | CCM_CLPCR_LPM(0);
  SCB_SCR &= ~SCB_SCR_SLEEPDEEP;
#elif POWER_MODE == POWER_MODE_WAIT
  // WAIT: CPU clocks gated, less aggressive than STOP.
  // NXP recommends INT_MEM_CLK_LPM for WAIT mode.
  CCM_CGPR |= CCM_CGPR_INT_MEM_CLK_LPM;
  CCM_CLPCR = (savedCLPCR & ~CCM_CLPCR_LPM(3)) |
              CCM_CLPCR_LPM(1) |
              CCM_CLPCR_ARM_CLK_DIS_ON_LPM;
  SCB_SCR &= ~SCB_SCR_SLEEPDEEP;
#else
  // STOP: experimental. This alone does not implement every silicon-errata
  // workaround for a production-quality suspend/resume sequence.
  CCM_CLPCR = (savedCLPCR & ~CCM_CLPCR_LPM(3)) |
              CCM_CLPCR_LPM(2) |
              CCM_CLPCR_ARM_CLK_DIS_ON_LPM |
              CCM_CLPCR_BYPASS_LPM_HS0 |
              CCM_CLPCR_BYPASS_LPM_HS1;
  SCB_SCR |= SCB_SCR_SLEEPDEEP;
#endif

  // D0 marks the entire low-power routine, not a continuous WFI instruction.
  digitalWriteFast(MARKER_PIN, HIGH);
  asm volatile("dsb" ::: "memory");
  asm volatile("isb" ::: "memory");

  // Atomic check and WFI: PRIMASK is set when checking. Pending enabled
  // IRQs can wake WFI; handlers run after interrupts() below.
  while (!wakeConditionMet()) {
    asm volatile("dsb" ::: "memory");
    asm volatile("wfi");
    interrupts();       // service the pending GPIO4/GPT2 IRQ
    noInterrupts();     // atomically check whether our source fired
  }

  bool pinWakeSeen = false;
#if USE_PIN_WAKE
  pinWakeSeen = pinTriggered || wakePinIsLow();
#endif
  bool timerWakeSeen = false;
#if USE_GPT2_WAKE
  timerWakeSeen = timerExpired;
#endif

  // Restore the low-power configuration before restarting normal tasks.
  SCB_SCR = savedSCR;
  CCM_CLPCR = savedCLPCR;
  CCM_CGPR = savedCGPR;

#if USE_GPT2_WAKE
  stopGPT2Wakeup(savedCCGR0);
#endif

#if POWER_MODE != POWER_MODE_CPU_WFI
  GPC_IMR1 = savedIMR1;
  GPC_IMR2 = savedIMR2;
  GPC_IMR3 = savedIMR3;
  GPC_IMR4 = savedIMR4;
  GPC_IMR5 = savedIMR5;
#endif

  SYST_CSR = savedSysTick;

  // Show source and complete sleep exit. D2/D3 are 150-ms pulses to remain
  // visible at a 1 kHz PPK2 sampling rate.
  digitalWriteFast(PIN_WAKE_DIAG_PIN, pinWakeSeen ? HIGH : LOW);
  digitalWriteFast(GPT_WAKE_DIAG_PIN, timerWakeSeen ? HIGH : LOW);
  digitalWriteFast(MARKER_PIN, LOW);

  interrupts();
  delay(150);
  digitalWriteFast(PIN_WAKE_DIAG_PIN, LOW);
  digitalWriteFast(GPT_WAKE_DIAG_PIN, LOW);

  wakeCount++;
}

// ============================================================================
// 7. APPLICATION TASK: HEARTBEAT PRESERVES ITS STATE THROUGH SLEEP
// ============================================================================

void runCpuTask() {
  cpuTaskCount++;
  uint32_t now = millis();
  if ((uint32_t)(now - lastLedToggleMs) >= LED_BLINK_INTERVAL_MS) {
    lastLedToggleMs = now;
    ledState = !ledState;
    digitalWriteFast(LED_PIN, ledState);
  }
}

void setup() {
  pinMode(MARKER_PIN, OUTPUT);
  pinMode(IRQ_SEEN_PIN, OUTPUT);
  pinMode(PIN_WAKE_DIAG_PIN, OUTPUT);
  pinMode(GPT_WAKE_DIAG_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWriteFast(MARKER_PIN, LOW);
  digitalWriteFast(IRQ_SEEN_PIN, LOW);
  digitalWriteFast(PIN_WAKE_DIAG_PIN, LOW);
  digitalWriteFast(GPT_WAKE_DIAG_PIN, LOW);
  digitalWriteFast(LED_PIN, LOW);

  // Visible marker self-check before the first sleep.
  digitalWriteFast(MARKER_PIN, HIGH);
  delay(300);
  digitalWriteFast(MARKER_PIN, LOW);
  delay(300);

#if USE_PIN_WAKE
  configureLowPowerWakePin();
#endif
#if USE_GPT2_WAKE
  attachInterruptVector(IRQ_GPT2, gpt2_isr);
  NVIC_DISABLE_IRQ(IRQ_GPT2);
  NVIC_CLEAR_PENDING(IRQ_GPT2);
#endif

  delay(3000); // allow USB upload before unplugging USB for measurements
  lastLedToggleMs = millis();
  lastSleepCycleMs = millis();
}

void loop() {
  runCpuTask();
  uint32_t now = millis();

  bool readyForSleep = true;
#if USE_PIN_WAKE
  readyForSleep = !wakePinIsLow();
  if (!readyForSleep) lastSleepCycleMs = now;
#endif

  if (readyForSleep &&
      (uint32_t)(now - lastSleepCycleMs) >= ACTIVE_BEFORE_SLEEP_MS) {
    lastSleepCycleMs = now;
    enterLowPower(SLEEP_SECONDS);
    lastSleepCycleMs = millis();
  }

  delay(1);
}
