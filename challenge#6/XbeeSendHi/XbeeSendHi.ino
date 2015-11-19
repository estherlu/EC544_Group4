
#include <XBee.h>
#include <SoftwareSerial.h>

SoftwareSerial Xbee(2,3);

void setup() {  

  Xbee.begin(9600);
  
}


void loop() {
  //Xbee.write("hi");
  if(Xbee.available())
  {
 Serial.println("Recieving");}
 while(Xbee.available()){
  
 Serial.write(Xbee.read());
  
  }
delay(1000);

}


