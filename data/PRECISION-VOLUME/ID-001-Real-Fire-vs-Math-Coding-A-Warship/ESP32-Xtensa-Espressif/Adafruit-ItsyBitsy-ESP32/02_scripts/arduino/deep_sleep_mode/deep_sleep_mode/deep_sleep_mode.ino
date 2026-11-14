/*
  TrueSpecLab: ESP32 Deep Sleep Forensic Test
  Turns off the main CPU and Radios. Wakes via RTC Timer.
*/
#define uS_TO_S_FACTOR 1000000ULL  // Conversion factor for microseconds to seconds
#define TIME_TO_SLEEP  10          // Time ESP32 will go to sleep (in seconds)

RTC_DATA_ATTR int bootCount = 0;   // RTC memory survives deep sleep

void setup() {
  Serial.begin(115200);
  delay(1000); // Allow serial to initialize

  bootCount++;
  Serial.println("Boot number: " + String(bootCount));
  Serial.println("Shutting down main CPU. Entering Deep Sleep for 10s...");
  Serial.flush(); // Ensure message sends before power is cut

  // Configure the RTC timer to wake the board up
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

  // Trigger Deep Sleep
  esp_deep_sleep_start();
  
  // Anything past this line will NEVER be executed.
}

void loop() {
  // Empty. Waking from Deep Sleep triggers setup() again.
}
