#include <Wire.h>
 #include <Time.h>
 #include <PID_v1.h>
 #include <Servo.h>
 #include<SoftwareSerial.h>
 #define    LIDARLite_ADDRESS   0x62          // Default I2C Address of LIDAR-Lite.
 #define    RegisterMeasure     0x00          // Register to write to initiate ranging.
 #define    MeasureValue        0x04          // Value to initiate ranging.
 #define    RegisterHighLowB    0x8f          // Register to get both High and Low bytes in 1 call.
 
  //SimpleTimer timer;
 Servo wheels; // servo for turning the wheels
 Servo esc; // not actually a servo, but controlled like one!
 SoftwareSerial XBee(2, 3); // RX, TX
 
 bool startup = true; // used to ensure startup only happens once
 int startupDelay = 1000; // time to pause at each calibration step
 double maxSpeedOffset = 45; // maximum speed magnitude, in servo 'degrees'
 double maxWheelOffset = 85; // maximum wheel turn magnitude, in servo 'degrees'
 time_t t = 0;
 int pinVal = 0;
 //int WSpin = 13;
 int cmnd = 49; //Command to start/stop the car remotely
 
 int l_0,Fl_0 ;
 int l_1;
 int dist,Firstdist,distLast,distAvg;
 long duration;
 double setPt, Input, Output;
 int Kp = 1.8;     // initial 2
 int Ki = 0.05;  // initial 0.05
 int Kd = 0.5;   // initial 0.5
 float angle;
 int HMC6352SlaveAddress = 0x42;
 int HMC6352ReadAddress = 0x41; //"A" in hex, A command is: 
 int headingValue;
 
 //int inchesLast_1 = 0;
 //int inchesLast_2 = 0;
 long distanceCm1,distanceCm2,distanceCmLast1,distanceCmLast2,distanceCmLast11,distanceCmLast22;
   
 PID PIDleft(&Input, &Output, &setPt, Kp, Ki, Kd, DIRECT);
 const int TRIG_PIN1 = 4;
 const int ECHO_PIN1 = 5;
 const int TRIG_PIN2 = 12;
 const int ECHO_PIN2 = 13;
 float headingSum;
 float headingInt; 
 //float headingIntObj;
 //float headingIntObj1;
 
 void repeatMe() {
      wheels.write(110);
 }
 long CalculateUltrasonic1(){
   digitalWrite(TRIG_PIN1, LOW);
   delayMicroseconds(2);
   digitalWrite(TRIG_PIN1, HIGH);
   delayMicroseconds(10);
   digitalWrite(TRIG_PIN1, LOW);
   long duration1 = pulseIn(ECHO_PIN1,HIGH);
  
   // convert the time into a distance
   return duration1 / 29.1 / 2 ;
 }
 
 long CalculateUltrasonic2(){
   digitalWrite(TRIG_PIN2, LOW);
   delayMicroseconds(2);
   digitalWrite(TRIG_PIN2, HIGH);
   delayMicroseconds(10);
   digitalWrite(TRIG_PIN2, LOW);
   long duration2 = pulseIn(ECHO_PIN2,HIGH);
  
   // convert the time into a distance
   return duration2 / 29.1 / 2 ;
 }
 void setup() {
   Serial.begin(9600); //Opens serial connection at 9600bps.
   XBee.begin(9600);
   wheels.attach(8); // initialize wheel servo to Digital IO Pin #8
   esc.attach(9); // initialize ESC to Digital IO Pin #9
     //If you're re-uploading code via USB while leaving the ESC powered on, 
    //  you don't need to re-calibrate each time, and you can comment this part out.
    
   pinMode(TRIG_PIN1,OUTPUT);
   pinMode(ECHO_PIN1,INPUT);
   pinMode(TRIG_PIN2,OUTPUT);
   pinMode(ECHO_PIN2,INPUT);
   
   // timer.setInterval(1500, repeatMe);
   HMC6352SlaveAddress = HMC6352SlaveAddress >> 1; // I know 0x42 is less than 127, but this is still required   
   calibrateESC();
   
   //pinMode(WSpin, INPUT);
   //t = millis();
   //pinVal = digitalRead(WSpin);
   Wire.begin(); // Opens & joins the irc bus as master
 
   //initialize linked variables
  digitalWrite(TRIG_PIN1, LOW);
   delayMicroseconds(2);
   digitalWrite(TRIG_PIN1, HIGH);
   delayMicroseconds(10);
   digitalWrite(TRIG_PIN1, LOW);
   duration = pulseIn(ECHO_PIN1,HIGH);
  
   // convert the time into a distance
   Firstdist = duration / 29.1 / 2 ;
          
      
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
   //int sensor_1, inches_1 /*sensor_2, inches_2, x*/
  
   
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

      if(cmnd==49||cmnd== 50)
   {
   distLast = dist;
   /* Read Data */
   
   //Ultrasonic sensor 1
   // convert the time into a distance
   distanceCmLast11=distanceCmLast1;
   distanceCmLast1=distanceCm1;
   distanceCm1 = CalculateUltrasonic1() ;
   if (distanceCm1 <= 0){
     Serial.println("Out of range");
   }
   else {
     Serial.print(distanceCm1);
     Serial.print("cm Ultrasonic sensor 1");
     Serial.println();
   }
  //Ultrasonic sensor 2 
   // convert the time into a distance
   distanceCmLast22=distanceCmLast2;
   distanceCmLast2=distanceCm2;
   distanceCm2 = CalculateUltrasonic2();
   if (distanceCm2 <= 0){
     Serial.println("Out of range");
   }
   else {
     Serial.print(distanceCm2);
     Serial.print("cm Ultrasonic sensor 2");
     Serial.println();
   }
 
   //LIDAR
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
 
   Serial.println("Dist ");
   Serial.println(dist);
   
   if (dist < 0) {
     dist = l_0;
   }
   //dist = 0.7 * dist + 0.3 * l_0;
   l_0 = dist;
       
   
  // if (cmnd == 49 or cmnd == 50)
   //{
    /* distAvg=(dist+distLast)/2;
     if ((distAvg <= 60)&&(dist>=19)) 
     { 
       esc.write(90);
     }
     else esc.write(77);*/
 
     if(dist > 0 && dist < 100)
       esc.write(90);
     else {  
       esc.write(78);
     //PID Computation
     if((cmnd==49)&&(distanceCm1<300)) {
       Input = (distanceCm1>100 && distanceCm1 <140)? setPt: distanceCm1;
       if (distanceCm1 >140) setPt = distanceCm1 - 10;
       // use PID loop
       PIDleft.Compute();
       wheels.write(110 + Output);
     }
       //Angle Calculation
     //angle=acos((distanceCm2-distanceCm1)/30);
       //turning phase
    
     if ((dist>150 && dist < 245 && distLast >150 && distLast<245) && (cmnd==49)) 
     {
       cmnd=50; 
       wheels.write(40); //Turn right
       esc.write(78);
       Serial.println("Turn RIGHT **");
     }
       //headingIntObj = headingInt +90;
    if (cmnd==50 && 
         ((distanceCm2-distanceCm1)>0 || abs(distanceCm2-distanceCm1)<5)&&
         ((distanceCmLast2-distanceCmLast1)>0 || abs(distanceCmLast2-distanceCmLast1)<5) && dist > 300) {
       cmnd = 49;
       wheels.write(110);
       esc.write(77);
       long tmpsetPt = CalculateUltrasonic1();
       setPt = (tmpsetPt>40) ? tmpsetPt : 55;
       delay(100);
       Serial.println("Going Straight");
     }
    // delay(1000);
   }
   }
 }
 
