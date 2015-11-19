int state=0;
void setup() {
 Serial.begin(9600);
  pinMode(7, OUTPUT);

  pinMode(8, INPUT);
}

void loop() {
  
   state=digitalRead(8);
 if(state==1)
  {digitalWrite(7, HIGH);
  }
 Serial.println(state);
 delay(300);
}
