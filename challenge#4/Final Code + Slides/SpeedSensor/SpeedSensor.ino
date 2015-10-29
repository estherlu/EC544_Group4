#include <Time.h>
#include<SoftwareSerial.h>
SoftwareSerial XBee(2, 3); // RX, TX

time_t t = 0;
int pinVal = 0;
int WSpin = 13;

void setup() {
 Serial.begin(9600);
 XBee.begin(9600);
  // put your setup code here, to run once:
 pinMode(WSpin, INPUT);
 //attachInterrupt(digitalPinToInterrupt(13), doEncoderLeft, CHANGE);
 t = millis();
 pinVal = digitalRead(WSpin);
}
void loop() {

//Wheel Speed Sensor
  int in = digitalRead(WSpin);
  if (pinVal != in) 
  { 
      pinVal = in;
      int out = millis() - t;
      //float wheelSpeed = (55.9/24.0 * 1000/out);
      t = millis();
      //XBee.println(wheelSpeed);
      XBee.println(out);
      //Serial.println(millis()-t);
      //Serial.println(t);
      //Serial.println("------------------");
   }

  // delay (100);

}

void doEncoderLeft(){
  Serial.println(0);
  t = now();
}
