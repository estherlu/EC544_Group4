#include <Wire.h>
#include <Time.h>
#include <PID_v1.h>
#include <Servo.h>
#include<SoftwareSerial.h>
#define    LIDARLite_ADDRESS   0x62          // Default I2C Address of LIDAR-Lite.
#define    RegisterMeasure     0x00          // Register to write to initiate ranging.
#define    MeasureValue        0x04          // Value to initiate ranging.
#define    RegisterHighLowB    0x8f          // Register to get both High and Low bytes in 1 call.

 
Servo wheels; // servo for turning the wheels
Servo esc; // not actually a servo, but controlled like one!
SoftwareSerial XBee(2, 3); // RX, TX

bool startup = true; // used to ensure startup only happens once
int startupDelay = 1000; // time to pause at each calibration step
double maxSpeedOffset = 45; // maximum speed magnitude, in servo 'degrees'
double maxWheelOffset = 85; // maximum wheel turn magnitude, in servo 'degrees'

time_t t = 0;
int pinVal = 0;
int WSpin = 13;
int cmnd = 49; //Command to start/stop the car remotely

int l_0,Fl_0 ;
int l_1;
int dist,Firstdist;
double setPt, Input, Output;
int Kp = 1.8;     // initial 2
int Ki = 0.05;  // initial 0.05
int Kd = 0.5;   // initial 0.5

int inchesLast_1 = 0;
int inchesLast_2 = 0;

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
  
  pinMode(WSpin, INPUT);
  t = millis();
  pinVal = digitalRead(WSpin);
  Wire.begin(); // Opens & joins the irc bus as master

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
     
  setPt = Firstdist;
  // Serial.println("First Distance"); 
  //Serial.print(Firstdist);
  PIDleft.SetOutputLimits(-50, 50);

  //turn on PID loop
  PIDleft.SetMode(AUTOMATIC);

  esc.write(80);
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
  int sensor_1, inches_1, sensor_2, inches_2, x;
  
  // read the analog output of the EZ1 from analog input 0
  sensor_1 = analogRead(0);
  sensor_2 = analogRead(2);
  
  // convert the sensor reading to inches
  inches_1 = sensor_1 / 2; //512 = 254 inches,
  inches_2 = sensor_2 / 2; //512 = 254 inches,

  if (XBee.available())
  {
     int tmp = XBee.read();
     if (tmp == 48 || tmp == 49)
     {
        cmnd = tmp;
        Serial.print("xbee");
        Serial.println(cmnd);
     }
     if(cmnd==49) //start
     {
       esc.write(75);
       Serial.println("Start");
     }
     if(cmnd==48)
     {
        esc.write(90);
       Serial.println("Stop");
     }
  }

 if (cmnd == 49)
  {
  
    if ((inches_1 <= 15 and inchesLast_1 <= 15) || (inches_2 <= 15 and inchesLast_2 <= 15)) esc.write(90);
    else esc.write(75);
    inchesLast_1 = inches_1;
    inchesLast_2 = inches_2;
    
    //print out the decimal result
    //Serial.print("EZ1: ");
    Serial.println(inches_1,DEC);
    Serial.println(inches_2,DEC);
  
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
    
    if (dist < 0 || dist > 130) {
      dist = l_0;
    }
    dist = 0.7 * dist + 0.3 * l_0;
    l_0 = dist;
            
    Input = dist;
  
    // use PID loop
    PIDleft.Compute();
    wheels.write(110 + Output);
  
     //Print Distance
      
    //Serial.print("Dist0: ");
    //Serial.print(dist);
    //Serial.print(", PID0: ");
    //Serial.println(Output);
 }

  //Wheel Speed Sensor
  //int in = digitalRead(WSpin);
  //if (pinVal != in) 
  //{ 
      //pinVal = in;
      //int out = millis() - t;
      //t = millis();
      //XBee.print("Speed: ");
      //XBee.println(out);
      //Serial.println(millis()-t);
      //Serial.println(t);
      //Serial.println("------------------");
   //}
   
    delay(100);
  }
