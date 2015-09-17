#include <SoftwareSerial.h>
#include "LowPower.h" // import the lowpoer library
SoftwareSerial XBee(2, 3); // RX, TX

void setup() {
  // put your setup code here, to run once:
  XBee.begin(9600);
  Serial.begin(9600);
}

void loop() {
  
  int ThermistorPin = 0; // Analog input pin for thermistor voltage
  int Vo; // Integer value of voltage reading
  float R = 10000.0; // Fixed resistance in the voltage divider
  float logRt,Rt,T; //Values for the Steinhart-Hart equation
  float c1 = 1.0009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;//Coef for the Steinhart-Hart equation
  
  Vo = analogRead(ThermistorPin);
  
  Rt = R *( 1023.0 / (float)Vo - 1.0 );
  logRt = log(Rt);
  
  //Calculate temp using SH equation 
  T = ( 1.0 / (c1 + c2*logRt + c3*logRt*logRt*logRt ) ) - 273.15;

  //Convert temp to str to transmit  through XBee
  int intTemp = floor(T*100);
  char strTemp[15];
  sprintf(strTemp, "3: %d.%d\n", intTemp/100, intTemp%100);

  //Set the arduino to sleep so that it only takes reading every 5 min
  for (int i=0; i<8; i++)
        // Sleep for 8 s with ADC module and BOD module off 
        LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 

  XBee.write(strTemp);
  Serial.write("Sent a message...\n");
}
