#include <SoftwareSerial.h>
SoftwareSerial XBee(2, 3);
String str;
//int userIp;
int led1,led2,led3,led4; 
void setup() {
  // initialize digital pins 4,5,6,7 as an output.
  Serial.begin(9600); 
  XBee.begin(9600);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {

    //read initial status of LEDs and send them to server
    led1 = digitalRead(4);   //Read the status of LED1, on pin 4
    led2 = digitalRead(5);   //Read the status of LED2, on pin 5
    led3 = digitalRead(6);   //Read the status of LED3, on pin 6
    led4 = digitalRead(7);   //Read the status of LED4, on pin 7
  
    if (digitalRead(4) == HIGH)  
      led1 = 1;
    else
      led1 = 0;
  
    if (digitalRead(5) == HIGH)  
      led2 = 1;
    else
      led2 = 0;
  
    if (digitalRead(6) == HIGH)  
      led3 = 1;
    else
      led3 = 0;
  
    if (digitalRead(7) == HIGH)  
      led4 = 1;
    else
      led4 = 0;
  
    //Convert Status into string and send it via XBee
    char buf[4];
    sprintf(buf, "%d%d%d%d\n", led1, led2, led3, led4); // puts string into buffer
    XBee.write(buf);  
    delay(1000);

    // Serial.println("Enter 1 for On and 0 for OFF");
    //Parse the LED Status string received from the server
    if (XBee.available())
    {
      int tmp = 0;
      while (tmp != 48 && tmp != 49){
        tmp =XBee.read();
        led1 = tmp;
      }
      tmp = 0;
      while (tmp != 48 && tmp != 49){
        tmp =XBee.read();
        led2 = tmp;
      }
      tmp = 0;
      while (tmp != 48 && tmp != 49){
        tmp =XBee.read();
        led3 = tmp;
      }
      tmp = 0;
      while (tmp != 48 && tmp != 49){
        tmp =XBee.read();
        led4 = tmp;
      }
    
      Serial.print(led1);
      Serial.print(led2);
      Serial.print(led3);
      Serial.print(led4);
      Serial.println("**");

      
      //Turn the LEDs on/off based on the command sent by the server 
      //LED 1
      if(led1==49) //if server sent 1 (on)
      {
        digitalWrite(4, HIGH);   // turn the LED on (HIGH is the voltage level)
       }
      else if(led1==48)
      {
        digitalWrite(4, LOW);    // turn the LED off by making the voltage LOW
                     // wait for a second
      }
      //LED2
      if(led2==49)
      {
        digitalWrite(5, HIGH);   // turn the LED on (HIGH is the voltage level)
      }
      else if(led2==48)
      {
        digitalWrite(5, LOW);    // turn the LED off by making the voltage LOW
                     // wait for a second
      }
      //LED3
      if(led3==49)
      {
        digitalWrite(6, HIGH);   // turn the LED on (HIGH is the voltage level)
      }
      else if(led3==48)
      {
        digitalWrite(6, LOW);    // turn the LED off by making the voltage LOW
                     // wait for a second
      }
      //LED4
      if(led4==49)
      {
        digitalWrite(7, HIGH);   // turn the LED on (HIGH is the voltage level)
                      // wait for a second
      }
      else if(led4==48)
      {
        digitalWrite(7, LOW);    // turn the LED off by making the voltage LOW
                     // wait for a second
      }


      delay(1000);
  }
}

