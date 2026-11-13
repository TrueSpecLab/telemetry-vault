void setup() {
  // put your setup code here, to run once:
  //int result = myFunction(2, 3);

  // Set output pins
  pinMode(GPIO_NUM_22, OUTPUT); 
  pinMode(GPIO_NUM_21, OUTPUT);
  pinMode(GPIO_NUM_19, OUTPUT);
  pinMode(GPIO_NUM_4, OUTPUT); 
  pinMode(GPIO_NUM_26, OUTPUT);
  pinMode(GPIO_NUM_25, OUTPUT); 
  pinMode(GPIO_NUM_8, OUTPUT);
  pinMode(GPIO_NUM_20, OUTPUT); 
  pinMode(GPIO_NUM_15, OUTPUT);
  pinMode(GPIO_NUM_27, OUTPUT);
  pinMode(GPIO_NUM_5, OUTPUT);
  pinMode(GPIO_NUM_7, OUTPUT);
  pinMode(GPIO_NUM_14, OUTPUT);
  pinMode(GPIO_NUM_12, OUTPUT);

  // Drive outputs LOW
  digitalWrite(GPIO_NUM_22, LOW); 
  digitalWrite(GPIO_NUM_21, LOW);
  digitalWrite(GPIO_NUM_19, LOW);
  digitalWrite(GPIO_NUM_4, LOW); 
  digitalWrite(GPIO_NUM_26, LOW);
  digitalWrite(GPIO_NUM_25, LOW); 
  digitalWrite(GPIO_NUM_8, LOW);
  digitalWrite(GPIO_NUM_20, LOW); 
  digitalWrite(GPIO_NUM_15, LOW);
  digitalWrite(GPIO_NUM_27, LOW);
  digitalWrite(GPIO_NUM_5, LOW);
  digitalWrite(GPIO_NUM_7, LOW);
  digitalWrite(GPIO_NUM_14, LOW);
  digitalWrite(GPIO_NUM_12, LOW);

  //Serial.begin(115200);
  //Serial.println("Hello, This is the setup function. It runs once at the beginning.");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(100); // Delay for 100ms
  digitalWrite(GPIO_NUM_22, HIGH); // Turn the LED on
  delay(100);
  digitalWrite(GPIO_NUM_21, HIGH);
  delay(100);
  digitalWrite(GPIO_NUM_19, HIGH);
  delay(100);
  digitalWrite(GPIO_NUM_4, HIGH);
  delay(100);
  digitalWrite(GPIO_NUM_26, HIGH);
  delay(100);
  digitalWrite(GPIO_NUM_25, HIGH);
  delay(100);
  digitalWrite(GPIO_NUM_8, HIGH);
  delay(100);
  digitalWrite(GPIO_NUM_20, HIGH);
  delay(100);
  digitalWrite(GPIO_NUM_15, HIGH);
  delay(100);
  digitalWrite(GPIO_NUM_27, HIGH);
  delay(100);
  digitalWrite(GPIO_NUM_5, HIGH);
  delay(100);
  digitalWrite(GPIO_NUM_7, HIGH);
  delay(100);
  digitalWrite(GPIO_NUM_14, HIGH);
  delay(100);
  digitalWrite(GPIO_NUM_12, HIGH);

  delay(100);
  digitalWrite(GPIO_NUM_22, LOW); // Turn the LED off
  delay(100);
  digitalWrite(GPIO_NUM_21, LOW);
  delay(100);
  digitalWrite(GPIO_NUM_19, LOW);
  delay(100);
  digitalWrite(GPIO_NUM_4, LOW);
  delay(100);
  digitalWrite(GPIO_NUM_26, LOW);
  delay(100);
  digitalWrite(GPIO_NUM_25, LOW);
  delay(100);
  digitalWrite(GPIO_NUM_8, LOW);
  delay(100);
  digitalWrite(GPIO_NUM_20, LOW);
  delay(100);
  digitalWrite(GPIO_NUM_15, LOW);
  delay(100);
  digitalWrite(GPIO_NUM_27, LOW);
  delay(100);
  digitalWrite(GPIO_NUM_5, LOW);
  delay(100);
  digitalWrite(GPIO_NUM_7, LOW);
  delay(100);
  digitalWrite(GPIO_NUM_14, LOW);
  delay(100);
  digitalWrite(GPIO_NUM_12, LOW);

  //Serial.println("Hello, This is the loop function. It runs repeatedly.");
}