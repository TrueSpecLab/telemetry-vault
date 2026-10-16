void setup() {
  // put your setup code here, to run once:
  //int result = myFunction(2, 3);

  // Set output pins
  pinMode(21, OUTPUT); 
  pinMode(22, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT); 
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT); 
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT); 
  pinMode(13, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(0, OUTPUT);
  pinMode(23, OUTPUT);
  pinMode(24, OUTPUT);

  // Drive outputs LOW
  digitalWrite(21, LOW); 
  digitalWrite(22, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW); 
  digitalWrite(9, LOW);
  digitalWrite(10, LOW); 
  digitalWrite(11, LOW);
  digitalWrite(12, LOW); 
  digitalWrite(13, LOW);
  digitalWrite(4, LOW);
  digitalWrite(1, LOW);
  digitalWrite(0, LOW);
  digitalWrite(23, LOW);
  digitalWrite(24, LOW);

  Serial.begin(115200);
  Serial.println("Hello, This is the setup function. It runs once at the beginning.");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(100); // Delay for 100ms
  digitalWrite(21, HIGH); // Turn the LED on
  delay(100); 
  digitalWrite(22, HIGH);
  delay(100);
  digitalWrite(5, HIGH);
  delay(100);
  digitalWrite(6, HIGH);
  delay(100);
  digitalWrite(9, HIGH);
  delay(100);
  digitalWrite(10, HIGH);
  delay(100);
  digitalWrite(11, HIGH);
  delay(100);
  digitalWrite(12, HIGH);
  delay(100);
  digitalWrite(13, HIGH);
  delay(100);
  digitalWrite(4, HIGH);
  delay(100);
  digitalWrite(1, HIGH);
  delay(100);
  digitalWrite(0, HIGH);
  delay(100);
  digitalWrite(23, HIGH);
  delay(100);
  digitalWrite(24, HIGH);

  delay(100);
  digitalWrite(21, LOW); // Turn the LED off
  delay(100); 
  digitalWrite(22, LOW);
  delay(100);
  digitalWrite(5, LOW);
  delay(100);
  digitalWrite(6, LOW);
  delay(100);
  digitalWrite(9, LOW);
  delay(100);
  digitalWrite(10, LOW);
  delay(100);
  digitalWrite(11, LOW);
  delay(100);
  digitalWrite(12, LOW);
  delay(100);
  digitalWrite(13, LOW);
  delay(100);
  digitalWrite(4, LOW);
  delay(100);
  digitalWrite(1, LOW);
  delay(100);
  digitalWrite(0, LOW);
  delay(100);
  digitalWrite(23, LOW);
  delay(100);
  digitalWrite(24, LOW);

  Serial.println("Hello, This is the loop function. It runs repeatedly.");
}