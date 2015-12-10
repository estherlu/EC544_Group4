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
const int LED_TURN = 13; 
const int LED_STOP=12;
// CART variables
Servo wheels; // servo for turning the wheels
Servo esc; // not actually a servo, but controlled like one!
int startupDelay = 1000;
/////////// XBee variables ///////////////////
SoftwareSerial XBee(2, 3); // RX, TX
/////////////////////////////////////////////
////// LIDAR variables //////////////////////
int dist,distLast1, distLast2,distActual;
///////////// Ultrasonic variables //////////
long duration,Firstduration, distanceCm, distLast, distanceCmLast,l_0,FirstdistanceCm;
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
  pinMode(LED_TURN, OUTPUT);
  pinMode(LED_STOP, OUTPUT);
  pinMode(TRIG_PIN,OUTPUT);
  pinMode(ECHO_PIN,INPUT);
  Serial.begin(9600); //Opens serial connection at 9600bps.
  // Setup the xbee
  XBee.begin(9600);
  // Setup the cart
  wheels.attach(8); // initialize wheel servo to Digital IO Pin #8
  esc.attach(9); // initialize ESC to Digital IO Pin #9
  calibrateESC();
  esc.write(75);
  digitalWrite(LED_TURN, LOW);
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
  setPt = dist;
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

    Serial.print("U1 distance");
    Serial.println(distanceCm); 

    
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
    distActual=dist;
    
    
    Serial.print("Dist0: ");
    Serial.println(dist);
    Serial.println(distActual);
    /////////////////////////////////////////////////////////////////////
    ///////////////////// CONTROL PART //////////////////////////////////
    // Stop if see something close in ftont
    if ((distanceCm > 0 && distanceCm < 60) && (distanceCm > 0 && distanceCm < 60)) {
      esc.write(90);
      digitalWrite(LED_STOP, HIGH);
    }
    //  right for 4 seconds using the LIDAR

    else if(distanceCm>140 && distanceCm<220 && distanceCmLast>140 && distanceCmLast<220 &&  cmnd == 49 && (distActual<6 || distActual>400)){
      cmnd = 50;
      esc.write(80);
      wheels.write(50);
      digitalWrite(LED_TURN, HIGH);
      
      //setPt = dist;
      //cmnd = 49;
      //digitalWrite(LED_TURN, LOW);
    }
    else if (cmnd == 49){
      // Set Input for PID, run PID   
       /*if (dist < 0 || dist > 110) {
          dist = distLast;
      }*/
      //dist = 0.7 * distLast + 0.3 * distLast;
      PIDleft.Compute();
      wheels.write(110 + Output);
      esc.write(75);
    }
     digitalWrite(LED_STOP, LOW);
    //Print Distance
     //Serial.print("U1 distance");
     //Serial.println(distanceCm); 
     //Serial.print("Dist0: ");
     //Serial.println(dist);
    delay(100);
    distLast=dist;
    distanceCmLast=distanceCm;
  }
}

