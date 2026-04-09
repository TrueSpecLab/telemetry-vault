void setup() {
  // put your setup code here, to run once:
  //int result = myFunction(2, 3);

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

  Serial.begin(115200);
  Serial.println("Hello, This is the setup function. It runs once at the beginning.");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(100); // Delay for 100ms
  digitalWrite(GPIO_NUM_21, HIGH); // Turn the LED on
  delay(100); 
  digitalWrite(GPIO_NUM_22, HIGH);
  delay(100);
  digitalWrite(GPIO_NUM_19, HIGH);
  delay(100);
  digitalWrite(GPIO_NUM_23, HIGH);
  delay(100);
  digitalWrite(GPIO_NUM_18, HIGH);
  delay(100);
  digitalWrite(GPIO_NUM_5, HIGH);
  delay(100);
  digitalWrite(GPIO_NUM_10, HIGH);
  delay(100);
  digitalWrite(GPIO_NUM_9, HIGH);
  delay(100);
  digitalWrite(GPIO_NUM_4, HIGH);
  delay(100);
  digitalWrite(GPIO_NUM_0, HIGH);
  delay(100);
  digitalWrite(GPIO_NUM_2, HIGH);
  delay(100);
  digitalWrite(GPIO_NUM_15, HIGH);
  delay(100);
  digitalWrite(GPIO_NUM_13, HIGH);
  delay(100);
  digitalWrite(GPIO_NUM_12, HIGH);

  delay(100);
  digitalWrite(GPIO_NUM_21, LOW); // Turn the LED off
  delay(100); 
  digitalWrite(GPIO_NUM_22, LOW);
  delay(100);
  digitalWrite(GPIO_NUM_19, LOW);
  delay(100);
  digitalWrite(GPIO_NUM_23, LOW);
  delay(100);
  digitalWrite(GPIO_NUM_18, LOW);
  delay(100);
  digitalWrite(GPIO_NUM_5, LOW);
  delay(100);
  digitalWrite(GPIO_NUM_10, LOW);
  delay(100);
  digitalWrite(GPIO_NUM_9, LOW);
  delay(100);
  digitalWrite(GPIO_NUM_4, LOW);
  delay(100);
  digitalWrite(GPIO_NUM_0, LOW);
  delay(100);
  digitalWrite(GPIO_NUM_2, LOW);
  delay(100);
  digitalWrite(GPIO_NUM_15, LOW);
  delay(100);
  digitalWrite(GPIO_NUM_13, LOW);
  delay(100);
  digitalWrite(GPIO_NUM_12, LOW);

  Serial.println("Hello, This is the loop function. It runs repeatedly.");
}