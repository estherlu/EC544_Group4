#include <Wire.h>
#include <Time.h>
#include <PID_v1.h>
#include <Servo.h>
#include <SoftwareSerial.h>
#define    LIDARLite_ADDRESS   0x62          // Default I2C Address of LIDAR-Lite.
#define    RegisterMeasure     0x00          // Register to write to initiate ranging.
#define    MeasureValue        0x04          // Value to initiate ranging.
#define    RegisterHighLowB    0x8f          // Register to get both High and Low bytes in 1 call.

// Pin assignment
const int TRIG_PIN = 4;
const int ECHO_PIN = 5;
const int LED_PIN = 13; 
// CART variables
Servo wheels; // servo for turning the wheels
Servo esc; // not actually a servo, but controlled like one!
int startupDelay = 1000;
/////////// XBee variables ///////////////////
SoftwareSerial XBee(2, 3); // RX, TX
/////////////////////////////////////////////
////// LIDAR variables //////////////////////
int dist,distLast;
///////////// Ultrasonic variables //////////
long duration,Firstduration, distanceCm, distanceCmLast,l_0,FirstdistanceCm;
/////////////////////////////////////////////
//////////// State and PID variables ////////
int cmnd = 49; //Command to start/stop the car remotely
// cmnd map :
///////////// 49: Auto Straight; 50: Auto Turn right; 51: Remote
double setPt, Input, Output;
int Kp = 2;     // initial 2
int Ki = 0.05;  // initial 0.05
int Kd = 0.5;   // initial 0.5
PID PIDleft(&Input, &Output, &setPt, Kp, Ki, Kd, DIRECT);
/////////////////////////////////////////////

void setup() {
  // PIN setup
  pinMode(LED_PIN, OUTPUT);
  pinMode(TRIG_PIN,OUTPUT);
  pinMode(ECHO_PIN,INPUT);
  Serial.begin(9600); //Opens serial connection at 9600bps.
  // Setup the xbee
  XBee.begin(9600);
  // Setup the cart
  wheels.attach(8); // initialize wheel servo to Digital IO Pin #8
  esc.attach(9); // initialize ESC to Digital IO Pin #9
  calibrateESC();
  esc.write(77);
  digitalWrite(LED_PIN, LOW);
  cmnd = 49;
  // LIDAR setup
  Wire.begin(); // Opens & joins the irc bus as master
  // Ultrasonic setup
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  Firstduration = pulseIn(ECHO_PIN,HIGH);
  // convert the time into a distance
  FirstdistanceCm = Firstduration / 29.1 / 2 ; 
  // PID setup
  setPt = FirstdistanceCm;
  PIDleft.SetOutputLimits(-50, 50);
  PIDleft.SetMode(AUTOMATIC);
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
  if (cmnd == 49 || cmnd == 50)
  {
    // Measure ultrasonic
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    duration = pulseIn(ECHO_PIN,HIGH);
    // convert the time into a distance
    distanceCm = duration / 29.1 / 2 ;
    if(distanceCm<0 || distanceCm>110){
      distanceCm=distanceCmLast;
    }
    distanceCm=0.7*distanceCm + 0.3*distanceCmLast;
    distanceCmLast=distanceCm;
    
    // Measure LIDAR
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
    distLast=dist;
    /////////////////////////////////////////////////////////////////////
    ///////////////////// CONTROL PART //////////////////////////////////
    // Stop if see something close in front
    if (dist < 60 && distLast < 60) {
      esc.write(90);
    }
    // Turn right for 4 seconds using the LIDAR
    else if(dist>140 && dist<220 && distLast>140 && distLast<220 && cmnd == 49){
      cmnd = 50;
      esc.write(80);
      wheels.write(50);
      digitalWrite(LED_PIN, HIGH);
      delay(4000);
      //setPt = distanceCm;
      cmnd = 49;
      digitalWrite(LED_PIN, LOW);
    }
    else if (cmnd == 49){
      // Set Input for PID, run PID     
      Input = distanceCm;
      PIDleft.Compute();
      wheels.write(110 + Output);
    }

    //Print Distance
    //Serial.print("U1 distance");
    //Serial.println(distanceCm); 
    //Serial.print("Dist0: ");
    //Serial.println(dist);
    delay(100);
  }
}
