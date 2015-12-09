#include <Wire.h>
#include <Time.h>
#include <PID_v1.h>
#include <Servo.h>
#include<SoftwareSerial.h>
#define    LIDARLite_ADDRESS   0x62          // Default I2C Address of LIDAR-Lite.
#define    RegisterMeasure     0x00          // Register to write to initiate ranging.
#define    MeasureValue        0x04          // Value to initiate ranging.
#define    RegisterHighLowB    0x8f          // Register to get both High and Low bytes in 1 call.

long duration,Firstduration, distanceCm, distanceCmLast,l_0,FirstdistanceCm;
const int TRIG_PIN = 4;
const int ECHO_PIN = 5; 
Servo wheels; // servo for turning the wheels
Servo esc; // not actually a servo, but controlled like one!
SoftwareSerial XBee(2, 3); // RX, TX

bool startup = true; // used to ensure startup only happens once
int startupDelay = 1000; // time to pause at each calibration step
double maxSpeedOffset = 45; // maximum speed magnitude, in servo 'degrees'
double maxWheelOffset = 85; // maximum wheel turn magnitude, in servo 'degrees'

time_t t = 0;
//int pinVal = 0;
int WSpin = 13;
int cmnd = 49; //Command to start/stop the car remotely


int dist,distLast;
double setPt, Input, Output;
int Kp = 2;     // initial 2
int Ki = 0.05;  // initial 0.05
int Kd = 0.5;   // initial 0.5



PID PIDleft(&Input, &Output, &setPt, Kp, Ki, Kd, DIRECT);

void setup() {
 
  Serial.begin(9600); //Opens serial connection at 9600bps.
  XBee.begin(9600);
  wheels.attach(8); // initialize wheel servo to Digital IO Pin #8
  esc.attach(9); // initialize ESC to Digital IO Pin #9
  /*  If you're re-uploading code via USB while leaving the ESC powered on, 
   *  you don't need to re-calibrate each time, and you can comment this part out.
   */
  calibrateESC();
  pinMode(13, OUTPUT);
  pinMode(TRIG_PIN,OUTPUT);
  pinMode(ECHO_PIN,INPUT);
  pinMode(WSpin, INPUT);
 // t = millis();
 
  Wire.begin(); // Opens & joins the irc bus as master
/*
  //initialize linked variables
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
    Firstdist = Wire.read(); // receive high byte (overwrites previous reading)
    Firstdist = Firstdist << 8; // shift high byte to be high 8 bits
    Firstdist |= Wire.read(); // receive low byte as lower 8 bits
  }    
   */  
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  Firstduration = pulseIn(ECHO_PIN,HIGH);
 
  // convert the time into a distance
  FirstdistanceCm = Firstduration / 29.1 / 2 ; 
   
  setPt = FirstdistanceCm;
  // Serial.println("First Distance"); 
  //Serial.print(Firstdist);
  PIDleft.SetOutputLimits(-50, 50);

  //turn on PID loop
  PIDleft.SetMode(AUTOMATIC);

  esc.write(77);
}

/* Calibrate the ESC by sending a high signal, then a low, then middle.*/
void calibrateESC(){
    esc.write(180); // full backwards
    delay(startupDelay);
    esc.write(0); // full forwards
    delay(startupDelay);
    esc.write(90); // neutral
    delay(startupDelay);
    esc.write(90); // reset the ESC to neutral (non-moving) value
}

void loop() {


  if (cmnd == 49)
  {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    duration = pulseIn(ECHO_PIN,HIGH);
   
    // convert the time into a distance
    distanceCm = duration / 29.1 / 2 ;


  
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
   /* 
    if (dist < 0 || dist > 110) {
      dist = l_0;
    }
    dist = 0.7 * dist + 0.3 * l_0;
    l_0 = dist;
   */
    if(distanceCm<0 || distanceCm>110){
      distanceCm=l_0;
    }
    distanceCm=0.7*distanceCm + 0.3*l_0;
    l_0 = distanceCm;
           
    Input = distanceCm;
    
    // use PID loop
    PIDleft.Compute();
    wheels.write(110 + Output);

    if(dist>140 && dist<220 && distLast>140 && distLast<220){
      esc.write(80);
      wheels.write(50);
      digitalWrite(13, HIGH);
      delay(4000);
    }
     //Print Distance
     Serial.print("U1 distance");
    Serial.println(distanceCm);
    
    Serial.print("Dist0: ");
    Serial.println(dist);
   
  
    digitalWrite(13, LOW);
    distLast=dist;
    distanceCmLast=distanceCm;
    delay(100);
  }
}
