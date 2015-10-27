#include <Time.h>

time_t t = 0;
int pinVal = 0;
int WSpin = 13;

void setup() {
 Serial.begin(9600);
  // put your setup code here, to run once:
 pinMode(WSpin, INPUT);
 //attachInterrupt(digitalPinToInterrupt(13), doEncoderLeft, CHANGE);
 t = millis();
 pinVal = digitalRead(WSpin);
}
void loop() {

  //Serial.println(digitalRead(13));
  int in = digitalRead(WSpin);
  if (pinVal != in) {
    pinVal = in;
    Serial.println(millis()-t);
    //Serial.println(t);
    Serial.println("------------------");
    t = millis();
  }
  //delay(1);
  
  // put your main code here, to run repeatedly:

}

void doEncoderLeft(){
  Serial.println(0);
  t = now();
}
