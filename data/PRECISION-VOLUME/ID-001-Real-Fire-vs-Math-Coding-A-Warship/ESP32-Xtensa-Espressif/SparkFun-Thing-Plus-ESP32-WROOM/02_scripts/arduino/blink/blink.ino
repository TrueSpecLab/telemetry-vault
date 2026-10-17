void setup() {
  // put your setup code here, to run once:
  //int result = myFunction(2, 3);

  // Set output pins 
  pinMode(GPIO_NUM_12, OUTPUT);
  pinMode(GPIO_NUM_27, OUTPUT);
  pinMode(GPIO_NUM_33, OUTPUT); 
  pinMode(GPIO_NUM_15, OUTPUT);
  pinMode(GPIO_NUM_32, OUTPUT); 
  pinMode(GPIO_NUM_14, OUTPUT);
  pinMode(GPIO_NUM_22, OUTPUT); 
  pinMode(GPIO_NUM_23, OUTPUT);
  pinMode(GPIO_NUM_21, OUTPUT);
  pinMode(GPIO_NUM_17, OUTPUT);
  pinMode(GPIO_NUM_16, OUTPUT);
  pinMode(GPIO_NUM_19, OUTPUT);
  pinMode(GPIO_NUM_18, OUTPUT);
  pinMode(GPIO_NUM_5, OUTPUT);

  // Drive outputs LOW
  digitalWrite(GPIO_NUM_12, LOW);
  digitalWrite(GPIO_NUM_27, LOW);
  digitalWrite(GPIO_NUM_33, LOW); 
  digitalWrite(GPIO_NUM_15, LOW);
  digitalWrite(GPIO_NUM_32, LOW); 
  digitalWrite(GPIO_NUM_14, LOW);
  digitalWrite(GPIO_NUM_22, LOW); 
  digitalWrite(GPIO_NUM_23, LOW);
  digitalWrite(GPIO_NUM_21, LOW);
  digitalWrite(GPIO_NUM_17, LOW);
  digitalWrite(GPIO_NUM_16, LOW);
  digitalWrite(GPIO_NUM_19, LOW);
  digitalWrite(GPIO_NUM_18, LOW);
  digitalWrite(GPIO_NUM_5, LOW);

  Serial.begin(115200);
  Serial.println("Hello, This is the setup function. It runs once at the beginning.");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(100); // Delay for 100ms
  digitalWrite(GPIO_NUM_12, HIGH);
  delay(100);
  digitalWrite(GPIO_NUM_27, HIGH);
  delay(100);
  digitalWrite(GPIO_NUM_33, HIGH);
  delay(100);
  digitalWrite(GPIO_NUM_15, HIGH);
  delay(100);
  digitalWrite(GPIO_NUM_32, HIGH);
  delay(100);
  digitalWrite(GPIO_NUM_14, HIGH);
  delay(100);
  digitalWrite(GPIO_NUM_22, HIGH);
  delay(100);
  digitalWrite(GPIO_NUM_23, HIGH);
  delay(100);
  digitalWrite(GPIO_NUM_21, HIGH);
  delay(100);
  digitalWrite(GPIO_NUM_17, HIGH);
  delay(100);
  digitalWrite(GPIO_NUM_16, HIGH);
  delay(100);
  digitalWrite(GPIO_NUM_19, HIGH);
  delay(100);
  digitalWrite(GPIO_NUM_18, HIGH);
  delay(100);
  digitalWrite(GPIO_NUM_5, HIGH);

  delay(100);
  digitalWrite(GPIO_NUM_12, LOW); // Turn the LED off
  delay(100);
  digitalWrite(GPIO_NUM_27, LOW);
  delay(100);
  digitalWrite(GPIO_NUM_33, LOW);
  delay(100);
  digitalWrite(GPIO_NUM_15, LOW);
  delay(100);
  digitalWrite(GPIO_NUM_32, LOW);
  delay(100);
  digitalWrite(GPIO_NUM_14, LOW);
  delay(100);
  digitalWrite(GPIO_NUM_22, LOW);
  delay(100);
  digitalWrite(GPIO_NUM_23, LOW);
  delay(100);
  digitalWrite(GPIO_NUM_21, LOW);
  delay(100);
  digitalWrite(GPIO_NUM_17, LOW);
  delay(100);
  digitalWrite(GPIO_NUM_16, LOW);
  delay(100);
  digitalWrite(GPIO_NUM_19, LOW);
  delay(100);
  digitalWrite(GPIO_NUM_18, LOW);
  delay(100);
  digitalWrite(GPIO_NUM_5, LOW);

  Serial.println("Hello, This is the loop function. It runs repeatedly.");
}