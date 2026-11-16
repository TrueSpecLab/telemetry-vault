/*
  TrueSpec Lab: Teensy 4.0 Power-Optimized Active-Low Hold Test
  Uses WFI (Wait For Interrupt) to minimize idle power safely.
  Tears down USB subsystem during the idle window to eliminate suspend lag.
*/

#define WAKEUP_PIN     2           // Digital pin used for active-low trigger (Pin 2)
#define SLEEP_SECONDS  10          // Idle window duration

unsigned long cycleCount = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000); // Wait for native USB serial connection

  Serial.println("\n--- TrueSpec Lab: Teensy 4.0 Instant Power-Down Log ---");
  pinMode(WAKEUP_PIN, INPUT_PULLUP);
}

void loop() {
  cycleCount++;
  Serial.println("\n--- Active Cycle #" + String(cycleCount) + " ---");

  // --- 1. ACTIVE-LOW HOLD LOGIC ---
  if (digitalRead(WAKEUP_PIN) == LOW) {
    Serial.println("Input pin is LOW. Holding active state...");
    while (digitalRead(WAKEUP_PIN) == LOW) {
      asm("wfi"); 
    }
    Serial.println("\nInput pin went HIGH. Proceeding to idle cycle.");
  }

  // --- 2. INSTANT LOW-POWER IDLE WINDOW ---
  Serial.println("Entering low-power idle window (" + String(SLEEP_SECONDS) + "s)...");
  Serial.flush();

  // Explicitly shut down the USB serial subsystem. 
  // This immediately disconnects the USB PHY and shuts down internal high-speed clocks,
  // bypassing the host's 3-second suspend timeout and dropping current instantly.
  Serial.end();

  unsigned long startTime = millis();
  unsigned long durationMs = SLEEP_SECONDS * 1000UL;

  bool wokenByPin = false;

  while (millis() - startTime < durationMs) {
    if (digitalRead(WAKEUP_PIN) == LOW) {
      wokenByPin = true;
      break;
    }
    asm("wfi"); 
  }

  // --- 3. WAKEUP RE-INITIALIZATION ---
  // Bring the USB serial interface back online so logging resumes seamlessly
  Serial.begin(115200);
  
  if (wokenByPin) {
    Serial.println("\n[Wakeup] External trigger detected during idle window!");
  } else {
    Serial.println("\n[Wakeup] Idle timer expired.");
  }
}