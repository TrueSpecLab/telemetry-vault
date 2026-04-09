// LED PWM Signals: 
// - ledc_hs_sig_out0~7
// - ledc_ls_sig_out0~7
// 16 independent channels @80 MHz clock/RTC CLK. Duty accuracy: 16 bits.

// the number of the LED pin
//const int ledPin = 21;  // 16 corresponds to GPIO16

// setting PWM properties
const int freq = 5000;
const int resolution = 12;

#define LED_PIN_1   GPIO_NUM_21
#define LED_PIN_2   GPIO_NUM_22
#define LED_PIN_3   GPIO_NUM_19
#define LED_PIN_4   GPIO_NUM_23
#define LED_PIN_5   GPIO_NUM_18
#define LED_PIN_6   GPIO_NUM_5
#define LED_PIN_7   GPIO_NUM_10
#define LED_PIN_8   GPIO_NUM_9
#define LED_PIN_9   GPIO_NUM_4
#define LED_PIN_10  GPIO_NUM_0
#define LED_PIN_11  GPIO_NUM_2
#define LED_PIN_12  GPIO_NUM_15
#define LED_PIN_13  GPIO_NUM_13
#define LED_PIN_14  GPIO_NUM_12

void setup() {
  // Set output pins
  pinMode(GPIO_NUM_21, OUTPUT); 
  pinMode(GPIO_NUM_22, OUTPUT);
  pinMode(GPIO_NUM_19, OUTPUT);
  pinMode(GPIO_NUM_23, OUTPUT); 
  pinMode(GPIO_NUM_18, OUTPUT);
  pinMode(GPIO_NUM_5, OUTPUT); 
  pinMode(GPIO_NUM_10, OUTPUT);
  pinMode(GPIO_NUM_9, OUTPUT); 
  pinMode(GPIO_NUM_4, OUTPUT);
  pinMode(GPIO_NUM_0, OUTPUT);
  pinMode(GPIO_NUM_2, OUTPUT);
  pinMode(GPIO_NUM_15, OUTPUT);
  pinMode(GPIO_NUM_13, OUTPUT);
  pinMode(GPIO_NUM_12, OUTPUT);

  // Drive outputs LOW
  digitalWrite(GPIO_NUM_21, LOW); 
  digitalWrite(GPIO_NUM_22, LOW);
  digitalWrite(GPIO_NUM_19, LOW);
  digitalWrite(GPIO_NUM_23, LOW); 
  digitalWrite(GPIO_NUM_18, LOW);
  digitalWrite(GPIO_NUM_5, LOW); 
  digitalWrite(GPIO_NUM_10, LOW);
  digitalWrite(GPIO_NUM_9, LOW); 
  digitalWrite(GPIO_NUM_4, LOW);
  digitalWrite(GPIO_NUM_0, LOW);
  digitalWrite(GPIO_NUM_2, LOW);
  digitalWrite(GPIO_NUM_15, LOW);
  digitalWrite(GPIO_NUM_13, LOW);
  digitalWrite(GPIO_NUM_12, LOW);

  // configure LED PWM
  ledcAttach(LED_PIN_1, freq, resolution);
  ledcAttach(LED_PIN_2, freq, resolution);
  ledcAttach(LED_PIN_3, freq, resolution);
  ledcAttach(LED_PIN_4, freq, resolution);
  ledcAttach(LED_PIN_5, freq, resolution);
  ledcAttach(LED_PIN_6, freq, resolution);
  ledcAttach(LED_PIN_7, freq, resolution);
  ledcAttach(LED_PIN_8, freq, resolution);
  ledcAttach(LED_PIN_9, freq, resolution);
  ledcAttach(LED_PIN_10, freq, resolution);
  ledcAttach(LED_PIN_11, freq, resolution);
  ledcAttach(LED_PIN_12, freq, resolution);
  ledcAttach(LED_PIN_13, freq, resolution);
  ledcAttach(LED_PIN_14, freq, resolution);

  Serial.begin(115200);

  // if you want to attach a specific channel, use the following instead
  //ledcAttachChannel(ledPin, freq, resolution, 0);
}

void loop(){

  unsigned long start_time1=millis(); //record start time

  // increase the LED brightness
  for(int dutyCycle = 0; dutyCycle <= 4095; dutyCycle++){   
    // changing the LED brightness with PWM
    ledcWrite(LED_PIN_1, dutyCycle);
    /*ledcWrite(LED_PIN_2, dutyCycle);
    ledcWrite(LED_PIN_3, dutyCycle);
    ledcWrite(LED_PIN_4, dutyCycle);
    ledcWrite(LED_PIN_5, dutyCycle);
    ledcWrite(LED_PIN_6, dutyCycle);
    ledcWrite(LED_PIN_7, dutyCycle);
    ledcWrite(LED_PIN_8, dutyCycle);
    ledcWrite(LED_PIN_9, dutyCycle);
    ledcWrite(LED_PIN_10, dutyCycle);
    ledcWrite(LED_PIN_11, dutyCycle);
    ledcWrite(LED_PIN_12, dutyCycle);
    ledcWrite(LED_PIN_13, dutyCycle);
    ledcWrite(LED_PIN_14, dutyCycle);*/
    delay(1);
  }

  unsigned long delta_t1 = millis() - start_time1; //the difference
  Serial.println("Fade In took:");
  Serial.println(delta_t1);
  Serial.println("milliseconds");

  unsigned long start_time2=millis(); //record start time

  // decrease the LED brightness
  for(int dutyCycle = 4095; dutyCycle >= 0; dutyCycle--){
    // changing the LED brightness with PWM
    ledcWrite(LED_PIN_1, dutyCycle);
    /*ledcWrite(LED_PIN_2, dutyCycle);
    ledcWrite(LED_PIN_3, dutyCycle);
    ledcWrite(LED_PIN_4, dutyCycle);
    ledcWrite(LED_PIN_5, dutyCycle);
    ledcWrite(LED_PIN_6, dutyCycle);
    ledcWrite(LED_PIN_7, dutyCycle);
    ledcWrite(LED_PIN_8, dutyCycle);
    ledcWrite(LED_PIN_9, dutyCycle);
    ledcWrite(LED_PIN_10, dutyCycle);
    ledcWrite(LED_PIN_11, dutyCycle);
    ledcWrite(LED_PIN_12, dutyCycle);
    ledcWrite(LED_PIN_13, dutyCycle);
    ledcWrite(LED_PIN_14, dutyCycle);*/
    delay(1);
  }

  unsigned long delta_t2 = millis() - start_time2; //the difference
  Serial.println("Fade Out took:");
  Serial.println(delta_t2);
  Serial.println("milliseconds");
}