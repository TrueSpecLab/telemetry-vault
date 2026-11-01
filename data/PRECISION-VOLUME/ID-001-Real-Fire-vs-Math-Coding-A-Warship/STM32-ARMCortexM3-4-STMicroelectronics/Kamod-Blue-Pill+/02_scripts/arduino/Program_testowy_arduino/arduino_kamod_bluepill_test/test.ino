
//#include <STM32RTC.h>

#define LED_PIN   PC13
#define SW_PIN    PA0

int message_period = 0;
int config_state = 0;
int ports_state = 0;
int pindex = 0;

STM32RTC& rtc = STM32RTC::getInstance();

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(SW_PIN, INPUT_PULLUP);
  rtc.begin(); // initialize RTC 24H format
}

// the loop function runs over and over again forever
void loop() {
  //digitalWrite(LED_PIN, HIGH);
  if (digitalRead(SW_PIN) == HIGH){
    if ((ports_state&4) > 0){
      digitalWrite(LED_PIN, HIGH);      
    } else {    
      digitalWrite(LED_PIN, LOW);      
    }

    if (config_state > 0){
      IO2input();
      config_state = 0;
    }

  } else {
    if ((ports_state&1) > 0){
      digitalWrite(LED_PIN, HIGH);      
    } else {    
      digitalWrite(LED_PIN, LOW);      
    }
    
    if (config_state == 0){
      IO2output();
      config_state = 1;
    }


    if (config_state == 1){
      IO2low();
      IOwriteIndexed(pindex, 1);
      pindex++;
      if (pindex > 26) pindex = 1;
    }
  }

  // check if plugged into a host
  if (message_period >= 11){
    message_period = 0;
    
    int tmt = rtc.getMinutes();
    Serial.print("KAmod BluePill+ RTC=");
    if (tmt < 10) Serial.print("0");
    Serial.print(tmt);
    
    tmt = rtc.getSeconds();
    Serial.print(":");
    if (tmt < 10) Serial.print("0");
    Serial.println(tmt);
  }
  
  delay(200); 
  message_period++;
  ports_state++;
}

void IO2output(){
  //pinMode(PA0, OUTPUT); //switch
  pinMode(PA1, OUTPUT);
  pinMode(PA2, OUTPUT);
  pinMode(PA3, OUTPUT);
  pinMode(PA4, OUTPUT);
  pinMode(PA5, OUTPUT);
  pinMode(PA6, OUTPUT);
  pinMode(PA7, OUTPUT);

  pinMode(PA8, OUTPUT);
  pinMode(PA9, OUTPUT);
  pinMode(PA10, OUTPUT);
  pinMode(PA15, OUTPUT);

  pinMode(PB0, OUTPUT);
  pinMode(PB1, OUTPUT);
  
  pinMode(PB3, OUTPUT);
  pinMode(PB4, OUTPUT);
  pinMode(PB5, OUTPUT);
  pinMode(PB6, OUTPUT);
  pinMode(PB7, OUTPUT);
  pinMode(PB8, OUTPUT);
  pinMode(PB9, OUTPUT);

  pinMode(PB10, OUTPUT);
  pinMode(PB11, OUTPUT);

  pinMode(PB12, OUTPUT);
  pinMode(PB13, OUTPUT);
  pinMode(PB14, OUTPUT);
  pinMode(PB15, OUTPUT);
  //pinMode(PC13, OUTPUT);  //led
}

void IO2input(){
  //pinMode(PA0, INPUT); //switch
  pinMode(PA1, INPUT);
  pinMode(PA2, INPUT);
  pinMode(PA3, INPUT);
  pinMode(PA4, INPUT);
  pinMode(PA5, INPUT);
  pinMode(PA6, INPUT);
  pinMode(PA7, INPUT);

  pinMode(PA8, INPUT);
  pinMode(PA9, INPUT);
  pinMode(PA10, INPUT);
  pinMode(PA15, INPUT);

  pinMode(PB0, INPUT);
  pinMode(PB1, INPUT);

  pinMode(PB3, INPUT);
  pinMode(PB4, INPUT);
  pinMode(PB5, INPUT);
  pinMode(PB6, INPUT);
  pinMode(PB7, INPUT);
  pinMode(PB8, INPUT);
  pinMode(PB9, INPUT);

  pinMode(PB10, INPUT);
  pinMode(PB11, INPUT);

  pinMode(PB12, INPUT);
  pinMode(PB13, INPUT);
  pinMode(PB14, INPUT);
  pinMode(PB15, INPUT);

  //pinMode(PC13, INPUT);  //led
}

void IO2low(){
  //digitalWrite(PA0, LOW); //switch
  digitalWrite(PA1, LOW);
  digitalWrite(PA2, LOW);
  digitalWrite(PA3, LOW);
  digitalWrite(PA4, LOW);
  digitalWrite(PA5, LOW);
  digitalWrite(PA6, LOW);
  digitalWrite(PA7, LOW);

  digitalWrite(PA8, LOW);
  digitalWrite(PA9, LOW);
  digitalWrite(PA10, LOW);
  digitalWrite(PA15, LOW);

  digitalWrite(PB0, LOW);
  digitalWrite(PB1, LOW);
  
  digitalWrite(PB3, LOW);
  digitalWrite(PB4, LOW);
  digitalWrite(PB5, LOW);
  digitalWrite(PB6, LOW);
  digitalWrite(PB7, LOW);
  digitalWrite(PB8, LOW);
  digitalWrite(PB9, LOW);

  digitalWrite(PB10, LOW);
  digitalWrite(PB11, LOW);

  digitalWrite(PB12, LOW);
  digitalWrite(PB13, LOW);
  digitalWrite(PB14, LOW);
  digitalWrite(PB15, LOW);

  //digitalWrite(PC13, LOW);   
}

void IO2high(){
  //digitalWrite(PA0, HIGH); //switch
  digitalWrite(PA1, HIGH);
  digitalWrite(PA2, HIGH);
  digitalWrite(PA3, HIGH);
  digitalWrite(PA4, HIGH);
  digitalWrite(PA5, HIGH);
  digitalWrite(PA6, HIGH);
  digitalWrite(PA7, HIGH);

  digitalWrite(PA8, HIGH);
  digitalWrite(PA9, HIGH);
  digitalWrite(PA10, HIGH);
  digitalWrite(PA15, HIGH);

  digitalWrite(PB0, HIGH);
  digitalWrite(PB1, HIGH);
  
  digitalWrite(PB3, HIGH);
  digitalWrite(PB4, HIGH);
  digitalWrite(PB5, HIGH);
  digitalWrite(PB6, HIGH);
  digitalWrite(PB7, HIGH);
  digitalWrite(PB8, HIGH);
  digitalWrite(PB9, HIGH);

  digitalWrite(PB10, HIGH);
  digitalWrite(PB11, HIGH);

  digitalWrite(PB12, HIGH);
  digitalWrite(PB13, HIGH);
  digitalWrite(PB14, HIGH);
  digitalWrite(PB15, HIGH);

  //digitalWrite(PC13, HIGH);   
}

void IOwriteIndexed(int index, int state){
  int pp = 1;

  switch (index){
    //case 0: pp = PA0; break;
    case 1: pp = PA1; break;
    case 2: pp = PA2; break;
    case 3: pp = PA3; break;
    case 4: pp = PA4; break;
    case 5: pp = PA5; break;
    case 6: pp = PA6; break;
    case 7: pp = PA7; break;

    case 8: pp = PB0; break;
    case 9: pp = PB1; break;
    case 10: pp = PB10; break;
    case 11: pp = PB11; break;

    case 12: pp = PB12; break;
    case 13: pp = PB13; break;
    case 14: pp = PB14; break;
    case 15: pp = PB15; break;

    case 16: pp = PA8; break;
    case 17: pp = PA9; break;
    case 18: pp = PA10; break;
    
    //case 19: pp = PA11; break
    //case 20: pp = PA12; break
    
    case 19: pp = PA15; break;

    case 20: pp = PB3; break;
    case 21: pp = PB4; break;
    case 22: pp = PB5; break;
    case 23: pp = PB6; break;

    case 24: pp = PB7; break;
    case 25: pp = PB8; break;
    case 26: pp = PB9; break;
  }

  pinMode(pp, OUTPUT);
  if (state > 0){
    digitalWrite(pp, HIGH);
  } else {
    digitalWrite(pp, LOW);
  }
}
