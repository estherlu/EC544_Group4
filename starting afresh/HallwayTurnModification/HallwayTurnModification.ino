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
const int TRIG_PIN2 = 12;
const int ECHO_PIN2 = 13;
const int LED_TURN = 11; 
const int LED_STOP=10;
// CART variables
Servo wheels; // servo for turning the wheels
Servo esc; // not actually a servo, but controlled like one!
int startupDelay = 1000;
/////////// XBee variables ///////////////////
SoftwareSerial XBee(2, 3); // RX, TX
/////////////////////////////////////////////
////// LIDAR variables //////////////////////
int dist,distLast1, distLast2;
///////////// Ultrasonic variables //////////
long duration,duration2,Firstduration, distanceCm, distanceCm2, distanceCmLast,l_0,FirstdistanceCm, distanceCmLast2;
/////////////////////////////////////////////
//////////// State and PID variables ////////
int cmnd = 49; //Command to start/stop the car remotely
int count = 2;
// cmnd map :
///////////// 49: Auto Straight; 50: Auto Turn right; 51: Remote
double setPt, Input, Output;
int Kp = 2;     // initial 2
int Ki = 0.05;  // initial 0.05
int Kd = 0.5;   // initial 0.5
PID PIDleft(&Input, &Output, &setPt, Kp, Ki, Kd, DIRECT);
/////////////////////////////////////////////
long CalculateUltrasonic1(){
   digitalWrite(TRIG_PIN, LOW);
   delayMicroseconds(2);
   digitalWrite(TRIG_PIN, HIGH);
   delayMicroseconds(10);
   digitalWrite(TRIG_PIN, LOW);
   long duration = pulseIn(ECHO_PIN,HIGH);
  
   // convert the time into a distance
   return duration / 29.1 / 2 ;
}
void setup() {
  // PIN setup
  pinMode(LED_TURN, OUTPUT);
  pinMode(LED_STOP, OUTPUT);
  pinMode(TRIG_PIN,OUTPUT);
  pinMode(ECHO_PIN,INPUT);
  pinMode(TRIG_PIN2,OUTPUT);
  pinMode(ECHO_PIN2,INPUT);
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
  distanceCm = 0;
  distanceCm2 = 0;
  count = 2;
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
  if (XBee.available())
   {
      int tmp = XBee.read();
     if(cmnd == 49 || cmnd==50)
     {
        if(tmp=='1')
        {
          cmnd=51;
          esc.write(90);
          wheels.write(110);   
        }
     }
     else if(cmnd==51)
     {
      if(tmp=='0')
      {
        cmnd=49;
        setPt=CalculateUltrasonic1();
        esc.write(80);
        wheels.write(110);
        
      }
      if(tmp=='2')//forward
      {
        wheels.write(110);
        esc.write(80);   
        
      }
      if(tmp=='3')//backwards
      {
        esc.write(100);
        wheels.write(110);   
      }
      if(tmp=='5') // right
      {
        wheels.write(60); 
       // esc.write(80);    
      }
      if(tmp=='4')//left
      {
        wheels.write(140); 
        //esc.write(80);  
          
      }
      if(tmp=='6')//stop
      {
        esc.write(90);  
      }
    }
   }  
  if (cmnd == 49 || cmnd == 50)
  {
    distanceCmLast = distanceCm;
    distanceCmLast2 = distanceCm2;
    // Measure ultrasonic 1
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    duration = pulseIn(ECHO_PIN,HIGH);
    // convert the time into a distance
    distanceCm = duration / 29.1 / 2 ;
    Serial.print("U1 is ");
    Serial.println(distanceCm);
    // Measure ultrasonic 2
    digitalWrite(TRIG_PIN2, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN2, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN2, LOW);
    duration2 = pulseIn(ECHO_PIN2,HIGH);
    // convert the time into a distance
    distanceCm2 = duration2 / 29.1 / 2 ;
    Serial.print("U2 is ");
    Serial.println(distanceCm2);
    // Measure LIDAR
    distLast2=distLast1;
    distLast1=dist;
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
    
    /////////////////////////////////////////////////////////////////////
    ///////////////////// CONTROL PART //////////////////////////////////
    // Stop if see something close in ftont
    if ((dist > 5 && dist < 60) && (distLast1 > 5 && distLast1 < 60)) {
      esc.write(90);
      digitalWrite(LED_STOP, HIGH);
    }
    else {
      if (cmnd == 49) {
        esc.write(76);
      }
      else {
        esc.write(77);
      }
      digitalWrite(LED_STOP, LOW);
      //Turn right
      if(
        (((dist>140 && dist<250 && distLast1>140 && distLast1<250 && (abs(distanceCm - distanceCmLast) > 35) 
        && (count == 0 || count  == 1 || count == 4 || count == 5 || count == 8))
        || (dist>140 && dist<300 && distLast1>140 && distLast1<300 && (abs(distanceCm - distanceCmLast) > 35) 
        && (count == 2 || count == 3 || count == 6 || count == 7 || count==9))) 
        &&  cmnd == 49)){
        cmnd = 50;
        esc.write(74);
        wheels.write(50);
        digitalWrite(LED_TURN, HIGH);
        count++;
        delay(2700);
      }
      else if (cmnd == 49){
        // Set Input for PID, run PID 
        if (((abs(distanceCm - distanceCmLast) < 50) && (dist<6 || dist>400)) || (distanceCm < 300)) {  
          Input = distanceCm;
          PIDleft.Compute();
          esc.write(75);
          wheels.write(110 + Output);
        }
        else {
          wheels.write(110);
          delay(3000); // Havent looked at trash can yet
        }
      } 
      else if (cmnd == 50 && ((abs(distanceCm - distanceCm2) < 5) || ((distanceCm2 - distanceCm) > 5)) && (abs(distanceCmLast - distanceCmLast2) < 5) || ((distanceCmLast2 - distanceCmLast) > 5)) {
        cmnd = 49;
        esc.write(75);
        wheels.write(110);
        if (count == 2 || count == 4  || count == 6 || count == 8) delay(1000);
        digitalWrite(TRIG_PIN, LOW);
        delayMicroseconds(2);
        digitalWrite(TRIG_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIG_PIN, LOW);
        long tmpduration = pulseIn(ECHO_PIN,HIGH);
        // convert the time into a distance
        long tmpdistanceCm = tmpduration / 29.1 / 2 ;
        setPt = (tmpdistanceCm < 10) ? (tmpdistanceCm + 15) : tmpdistanceCm;
        digitalWrite(LED_TURN, LOW);
      }
    }
      Serial.print("Dist0: ");
      Serial.println(dist);
    //delay(100);
  }
}
