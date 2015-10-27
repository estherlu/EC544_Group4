#include <Time.h>

time_t t = 0;

void doEncoderLeft(){
  Serial.print(now()-t);
  t = now();
}

void setup() {
 Serial.begin(9600);
  // put your setup code here, to run once:
 pinMode(2, INPUT);
 attachInterrupt(0, doEncoderLeft, CHANGE);
 t = now();
}
void loop() {
  // put your main code here, to run repeatedly:

}
