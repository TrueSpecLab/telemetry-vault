/*
  TrueSpec Lab: ESP32 Deep Sleep & External Wakeup Test
  Turns off the main CPU and Radios. Wakes via RTC Timer OR Digital Pin.
*/

#define uS_TO_S_FACTOR 1000000ULL   // Conversion factor for microseconds to seconds
#define TIME_TO_SLEEP  10           // Time ESP32 will go to sleep (in seconds)
#define WAKEUP_PIN     GPIO_NUM_12  // Must be an RTC-capable GPIO pin (e.g., GPIO 4, 12, 13, 14, 27, 33)

RTC_DATA_ATTR int bootCount = 0;    // RTC memory survives deep sleep

// Helper function to identify the wake source for diagnostic logging
void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();

  Serial.print("Wakeup cause: ");
  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0:     
      Serial.println("External signal using RTC_IO (Digital Pin trigger)"); 
      break;
    case ESP_SLEEP_WAKEUP_TIMER:    
      Serial.println("RTC Timer expired (10s interval)"); 
      break;
    default:                        
      Serial.printf("First boot or reset (Reason code: %d)\n", wakeup_reason); 
      break;
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000); // Allow serial to initialize

  bootCount++;
  Serial.println("\n--- TrueSpec Lab: Deep Sleep Forensic Log ---");
  Serial.println("Boot number: " + String(bootCount));
  
  // Check why we just woke up
  print_wakeup_reason();

  // Configure the External Wakeup (EXT0)
  // Wakes up when the pin goes LOW (0). 
  // Tip: Wire a pushbutton between WAKEUP_PIN and GND with an internal or external pull-up resistor.
  pinMode(WAKEUP_PIN, INPUT_PULLUP); // Holds pin HIGH; reads LOW when connected to GND

  // --- ACTIVE-LOW HOLD LOGIC ---
  // If the pin is currently LOW, stay in this loop and prevent sleep.
  if (digitalRead(WAKEUP_PIN) == LOW) {
    Serial.println("Input pin is LOW. Staying active...");
    while (digitalRead(WAKEUP_PIN) == LOW) {
      // Perform active tasks here while the pin is held low
      Serial.print(".");
      delay(500); 
    }
    Serial.println("\nInput pin went LOW. Proceeding to sleep cycle.");
  }

  // Configure the External Wakeup (EXT0) to trigger when the pin goes LOW (0)
  esp_sleep_enable_ext0_wakeup(WAKEUP_PIN, 0);

  // Configure the RTC Timer Wakeup as a fallback
  //esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

  Serial.println("Shutting down main CPU. Entering Deep Sleep...");
  Serial.println("Triggers: Timer (" + String(TIME_TO_SLEEP) + "s) OR Pin " + String(WAKEUP_PIN) + " going LOW.");
  Serial.flush(); // Ensure message sends before power is cut

  // Trigger Deep Sleep
  esp_deep_sleep_start();
  
  // Anything past this line will NEVER be executed.
}

void loop() {
  // Empty. Waking from Deep Sleep triggers setup() again.
}