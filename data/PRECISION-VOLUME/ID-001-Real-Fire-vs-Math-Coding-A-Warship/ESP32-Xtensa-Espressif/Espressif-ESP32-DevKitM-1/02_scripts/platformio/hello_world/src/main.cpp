#include <Arduino.h>

// put function declarations here:
//int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  //int result = myFunction(2, 3);

  pinMode(GPIO_NUM_5, OUTPUT); // Set GPIO2 as an output pin

  Serial.begin(115200);
  Serial.println("Hello, This is the setup function. It runs once at the beginning.");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000); // Delay for 1 second
  digitalWrite(GPIO_NUM_5, HIGH); // Turn the LED on
  delay(1000); // Delay for 1 second
  digitalWrite(GPIO_NUM_5, LOW); // Turn the LED off
  Serial.println("Hello, This is the loop function. It runs repeatedly.");
}

// put function definitions here:
/*int myFunction(int x, int y) {
  return x + y;
}*/