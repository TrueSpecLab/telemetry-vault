void setup() {
  // Set output pins
  pinMode(GPIO_NUM_21, OUTPUT); 
  pinMode(GPIO_NUM_17, OUTPUT);
  pinMode(GPIO_NUM_16, OUTPUT);
  pinMode(GPIO_NUM_19, OUTPUT); 
  pinMode(GPIO_NUM_18, OUTPUT);
  pinMode(GPIO_NUM_5, OUTPUT); 
  pinMode(GPIO_NUM_4, OUTPUT);
  pinMode(GPIO_NUM_25, OUTPUT); 
  pinMode(GPIO_NUM_26, OUTPUT);
  pinMode(GPIO_NUM_23, OUTPUT);
  pinMode(GPIO_NUM_22, OUTPUT);
  pinMode(GPIO_NUM_14, OUTPUT);
  pinMode(GPIO_NUM_32, OUTPUT);
  pinMode(GPIO_NUM_15, OUTPUT);

  // Drive outputs LOW
  digitalWrite(GPIO_NUM_21, LOW); 
  digitalWrite(GPIO_NUM_17, LOW);
  digitalWrite(GPIO_NUM_16, LOW);
  digitalWrite(GPIO_NUM_19, LOW); 
  digitalWrite(GPIO_NUM_18, LOW);
  digitalWrite(GPIO_NUM_5, LOW); 
  digitalWrite(GPIO_NUM_4, LOW);
  digitalWrite(GPIO_NUM_25, LOW); 
  digitalWrite(GPIO_NUM_26, LOW);
  digitalWrite(GPIO_NUM_23, LOW);
  digitalWrite(GPIO_NUM_22, LOW);
  digitalWrite(GPIO_NUM_14, LOW);
  digitalWrite(GPIO_NUM_32, LOW);
  digitalWrite(GPIO_NUM_15, LOW);
}

void loop(){
  // increase the LED brightness
  for(int dutyCycle = 0; dutyCycle <= 255; dutyCycle++){   
    // changing the LED brightness with PWM
    analogWrite(GPIO_NUM_21, dutyCycle);
    delay(15);
  }

  // decrease the LED brightness
  for(int dutyCycle = 255; dutyCycle >= 0; dutyCycle--){
    // changing the LED brightness with PWM
    analogWrite(GPIO_NUM_21, dutyCycle);
    delay(15);
  }
}