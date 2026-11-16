/*
  TrueSpec Lab: Teensy 4.0 Power-Optimized Active-Low Hold Test
  Uses WFI (Wait For Interrupt) and clock scaling to minimize idle draw.
*/

#define WAKEUP_PIN     2           // Digital pin used for active-low trigger (Pin 2)
#define SLEEP_SECONDS  10          // Idle window duration

unsigned long cycleCount = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000); // Wait for native USB serial

  Serial.println("\n--- TrueSpec Lab: Teensy 4.0 Power-Optimized Log ---");
  pinMode(WAKEUP_PIN, INPUT_PULLUP);
}

void loop() {
  cycleCount++;
  Serial.println("\n--- Active Cycle #" + String(cycleCount) + " ---");

  // --- 1. ACTIVE-LOW HOLD LOGIC ---
  if (digitalRead(WAKEUP_PIN) == LOW) {
    Serial.println("Input pin is LOW. Holding active state...");
    while (digitalRead(WAKEUP_PIN) == LOW) {
      // Keep full 600MHz performance active during explicit hold operations
      asm("wfi"); // Sleep CPU lightly while waiting for state change
    }
    Serial.println("\nInput pin went HIGH. Proceeding to idle cycle.");
  }

  // --- 2. LOW-POWER IDLE WINDOW ---
  Serial.println("Entering low-power idle window (" + String(SLEEP_SECONDS) + "s)...");
  Serial.flush();

  unsigned long startTime = millis();
  unsigned long durationMs = SLEEP_SECONDS * 1000UL;

  // Optional: Drop CPU clock speed to 24 MHz during the idle window to drop baseline current
  // (Teensy core function to scale clock dynamically)
  set_cpu_frequency(24000000);

  while (millis() - startTime < durationMs) {
    if (digitalRead(WAKEUP_PIN) == LOW) {
      Serial.println("\nExternal trigger detected during idle window!");
      break;
    }
    
    // WFI halts the Cortex-M7 pipeline until the next hardware timer tick (SysTick)
    asm("wfi"); 
  }

  // Restore full 600 MHz performance for active tasks
  set_cpu_frequency(600000000);
}