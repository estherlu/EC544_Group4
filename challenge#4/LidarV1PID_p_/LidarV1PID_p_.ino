

//#include <I2C.h>
#include <Wire.h>
#include <PID_v1.h>
#define    LIDARLite_ADDRESS   0x62          // Default I2C Address of LIDAR-Lite.
#define    RegisterMeasure     0x00          // Register to write to initiate ranging.
#define    MeasureValue        0x04          // Value to initiate ranging.
#define    RegisterHighLowB    0x8f          // Register to get both High and Low bytes in 1 call.


const int left = 12;
const int right = 13;
int l_0;
int l_1;
int dist;
int active_sensor = 0; //0 is left, 1 is right
// define variables we will be connecting
double setPt, Input, Output;
int Kp = 2;     // initial 2
int Ki = 0.05;  // initial 0.05
int Kd = 0.5;   // initial 0.5

PID PIDleft(&Input, &Output, &setPt, Kp, Ki, Kd, DIRECT);

void setup() {

  //initialize LIDAR's left on, right off
  pinMode(left, OUTPUT);
  pinMode(right, OUTPUT);
  digitalWrite(left, HIGH);
  digitalWrite(right, LOW);

  Serial.begin(9600); //Opens serial connection at 9600bps.
  Wire.begin(); // Opens & joins the irc bus as master
  delay(100); // Waits to make sure everything is powered up before sending or receiving data

  //initialize linked variables
  setPt = 40; //30
  PIDleft.SetOutputLimits(-50, 50);

  //turn on PID loop
  PIDleft.SetMode(AUTOMATIC);

}

void loop() {
  for (int i = 0; i < 2; i++) {
    //    active_sensor = i;
    switch (i) {
      case 0:
        digitalWrite(left, LOW);
        digitalWrite(right, LOW);
        delay(10);
        digitalWrite(left, HIGH);
        delay(10);
        break;
      case 1:
        digitalWrite(left, LOW);
        digitalWrite(right, LOW);
        delay(10);
        digitalWrite(right, HIGH);
        delay(10);
        break;
    }
    //dist array to average dists
    int distSum = 0;

    for (int j = 0; j < 5; j++) {

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
      
      switch (i) {
        case 0:
          if (dist < 0 || dist > 400) {
            dist = l_0;
          }
          dist = 0.7 * dist + 0.3 * l_0;
          l_0 = dist;
          break;
        case 1:
          if (dist < 0 || dist > 400) {
            dist = l_1;
          }
          dist = 0.7 * dist + 0.3 * l_1;
          l_1 = dist;
          break;
      }
    distSum = distSum + dist;
   // Serial.print(j);
    //Serial.print(" : ");
    //Serial.println(distSum);
    delay(100);
    }

    dist = distSum / 5;

    //input to PID
    Input = dist;

    // use PID loop
    PIDleft.Compute();
    delay(10);

    // Print Distance
    switch (i) {
      case 0:
        Serial.print("Dist0: ");
        Serial.print(dist);
        Serial.print(", PID0: ");
        Serial.println(Output);
        delay(1);
        break;
      case 1:
        Serial.print("Dist1: ");
        Serial.print(dist);
        Serial.print(", PID1: ");
        Serial.println(Output);
    }
    delay(20);
  }
}
