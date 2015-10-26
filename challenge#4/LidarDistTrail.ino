
//#include <I2C.h>
#include <Wire.h>
#include <PID_v1.h>
#define    LIDARLite_ADDRESS   0x62          // Default I2C Address of LIDAR-Lite.
#define    RegisterMeasure     0x00          // Register to write to initiate ranging.
#define    MeasureValue        0x04          // Value to initiate ranging.
#define    RegisterHighLowB    0x8f          // Register to get both High and Low bytes in 1 call.


int l_0;
int l_1;
int dist;
double Output;

void setup() {



  Serial.begin(9600); //Opens serial connection at 9600bps.
  Wire.begin(); // Opens & joins the irc bus as master
  delay(100); // Waits to make sure everything is powered up before sending or receiving data


}

void loop() {
  
      Wire.beginTransmission((int)LIDARLite_ADDRESS); // transmit to LIDAR-Lite
      Wire.write((int)RegisterMeasure); // sets register pointer to  (0x00)
      Wire.write((int)MeasureValue); // sets register pointer to  (0x00)
      Wire.endTransmission(); // stop transmitting

      delay(20); // Wait 20ms for transmit

      Wire.beginTransmission((int)LIDARLite_ADDRESS); // transmit to LIDAR-Lite
      Wire.write((int)RegisterHighLowB); // sets register pointer to (0x8f)
      Wire.endTransmission(); // stop transmitting

      delay(20); // Wait 20ms for transmit

      Wire.requestFrom((int)LIDARLite_ADDRESS, 2); // request 2 bytes from LIDAR-Lite

      if (2 <= Wire.available()) // if two bytes were received
      {
        dist = Wire.read(); // receive high byte (overwrites previous reading)
        dist = dist << 8; // shift high byte to be high 8 bits
        dist |= Wire.read(); // receive low byte as lower 8 bits
      }
      
          if (dist < 0 || dist > 400) {
            dist = l_0;
          }
          dist = 0.7 * dist + 0.3 * l_0;
          l_0 = dist;
       
        Serial.print("Dist0: ");
        Serial.print(dist);
        Serial.print(", PID0: ");
        Serial.println(Output);
        delay(1);
       
  
  
}
