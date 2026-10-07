void setup() {
  // put your setup code here, to run once:
  //int result = myFunction(2, 3);

  // Use either 'D9' or the global 'LED_BUILTIN' constant
  pinMode(D9, OUTPUT); 
  digitalWrite(D9, LOW); // Turn off the onboard LED

  // Set output pins
  pinMode(GPIO_NUM_3, OUTPUT); 
  pinMode(GPIO_NUM_26, OUTPUT);
  pinMode(GPIO_NUM_0, OUTPUT);
  pinMode(GPIO_NUM_14, OUTPUT); 
  pinMode(GPIO_NUM_13, OUTPUT);
  pinMode(GPIO_NUM_21, OUTPUT);
  pinMode(GPIO_NUM_19, OUTPUT); 
  pinMode(GPIO_NUM_17, OUTPUT); 
  pinMode(GPIO_NUM_16, OUTPUT);
  pinMode(GPIO_NUM_4, OUTPUT);
  pinMode(GPIO_NUM_12, OUTPUT);
  pinMode(GPIO_NUM_15, OUTPUT);
  pinMode(GPIO_NUM_23, OUTPUT);
  pinMode(GPIO_NUM_22, OUTPUT);

  // Drive outputs LOW
  digitalWrite(GPIO_NUM_25, LOW); 
  digitalWrite(GPIO_NUM_26, LOW);
  digitalWrite(GPIO_NUM_0, LOW);
  digitalWrite(GPIO_NUM_14, LOW); 
  digitalWrite(GPIO_NUM_13, LOW); 
  digitalWrite(GPIO_NUM_21, LOW);
  digitalWrite(GPIO_NUM_19, LOW);
  digitalWrite(GPIO_NUM_17, LOW); 
  digitalWrite(GPIO_NUM_16, LOW);
  digitalWrite(GPIO_NUM_4, LOW);
  digitalWrite(GPIO_NUM_12, LOW);
  digitalWrite(GPIO_NUM_15, LOW);
  digitalWrite(GPIO_NUM_23, LOW);
  digitalWrite(GPIO_NUM_22, LOW);

  Serial.begin(115200);
  Serial.println("Hello, This is the setup function. It runs once at the beginning.");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(100); // Delay for 100ms
  digitalWrite(GPIO_NUM_25, HIGH); // Turn the LED on
  delay(100); 
  digitalWrite(GPIO_NUM_26, HIGH);
  delay(100); 
  digitalWrite(GPIO_NUM_0, HIGH);
  delay(100); 
  digitalWrite(GPIO_NUM_14, HIGH);
  delay(100); 
  digitalWrite(GPIO_NUM_13, HIGH);
  delay(100); 
  digitalWrite(GPIO_NUM_21, HIGH);
  delay(100); 
  digitalWrite(GPIO_NUM_19, HIGH);
  delay(100); 
  digitalWrite(GPIO_NUM_17, HIGH);
  delay(100); 
  digitalWrite(GPIO_NUM_16, HIGH);
  delay(100); 
  digitalWrite(GPIO_NUM_4, HIGH);
  delay(100); 
  digitalWrite(GPIO_NUM_12, HIGH);
  delay(100); 
  digitalWrite(GPIO_NUM_15, HIGH);
  delay(100); 
  digitalWrite(GPIO_NUM_23, HIGH);
  delay(100); 
  digitalWrite(GPIO_NUM_22, HIGH);

  delay(100);
  digitalWrite(GPIO_NUM_25, LOW); // Turn the LED off
  delay(100);
  digitalWrite(GPIO_NUM_26, LOW);
  delay(100); 
  digitalWrite(GPIO_NUM_0, LOW);
  delay(100); 
  digitalWrite(GPIO_NUM_14, LOW);
  delay(100); 
  digitalWrite(GPIO_NUM_13, LOW);
  delay(100); 
  digitalWrite(GPIO_NUM_21, LOW);
  delay(100); 
  digitalWrite(GPIO_NUM_19, LOW);
  delay(100); 
  digitalWrite(GPIO_NUM_17, LOW);
  delay(100); 
  digitalWrite(GPIO_NUM_16, LOW);
  delay(100); 
  digitalWrite(GPIO_NUM_4, LOW);
  delay(100); 
  digitalWrite(GPIO_NUM_12, LOW);
  delay(100); 
  digitalWrite(GPIO_NUM_15, LOW);
  delay(100); 
  digitalWrite(GPIO_NUM_23, LOW);
  delay(100); 
  digitalWrite(GPIO_NUM_22, LOW);

  Serial.println("Hello, This is the loop function. It runs repeatedly.");
}