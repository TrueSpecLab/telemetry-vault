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