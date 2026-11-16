/*
  TrueSpec Lab: Feather M4 Express (ATSAMD51) Low-Power & Pin-Hold Test
  Sleeps via Watchdog Timer. Stays active as long as WAKEUP_PIN is LOW.
*/

#include <Adafruit_SleepyDog.h>

#define TIME_TO_SLEEP  10          // Sleep duration in seconds
#define WAKEUP_PIN     4           // Digital pin used for active-low trigger (e.g., Pin 4)

int cycleCount = 0;

void setup() {
  Serial.begin(115200);
  
  // Wait up to 3 seconds for native USB serial connection to establish on boot
  while (!Serial && millis() < 3000);

  Serial.println("\n--- TrueSpec Lab: Feather M4 Power Management Initialized ---");
  
  // Configure the wakeup pin with an internal pull-up (defaults HIGH, active LOW)
  pinMode(WAKEUP_PIN, INPUT_PULLUP);
  
  // Enable the watchdog timer framework
  Watchdog.enable(16000); 
}

void loop() {
  cycleCount++;
  Serial.println("\n--- Active Cycle #" + String(cycleCount) + " ---");

  // --- ACTIVE-LOW HOLD LOGIC ---
  // If the input pin is LOW, hold the CPU awake and prevent entering sleep
  if (digitalRead(WAKEUP_PIN) == LOW) {
    Serial.println("Input pin is LOW. Holding active state...");
    while (digitalRead(WAKEUP_PIN) == LOW) {
      // Perform active metrology, logging, or sensor polling tasks here
      Serial.print(".");
      delay(500);
    }
    Serial.println("\nInput pin went HIGH. Proceeding to sleep cycle.");
  }

  // --- LOW-POWER SLEEP STAGE ---
  Serial.println("Entering low-power sleep for " + String(TIME_TO_SLEEP) + " seconds...");
  Serial.flush(); // Ensure serial buffer is fully transmitted before sleeping

  // Watchdog.sleep puts the ATSAMD51 into a low-power standby state
  // TIME_TO_SLEEP is converted to milliseconds
  int sleepMS = Watchdog.sleep(TIME_TO_SLEEP * 1000);

  Serial.println("Woke up! Actual sleep duration: " + String(sleepMS) + " ms");
}