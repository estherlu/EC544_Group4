#include <SoftwareSerial.h>
SoftwareSerial XBee(2, 3); // RX, TX
#include "Time.h"
#include <SimpleTimer.h>


char id = '4';
char command = 'N';
bool leader = true;
bool infected = false;
int id_list[5];
unsigned long id_time[5] = {0, 0, 0, 0, 0};
unsigned long immune_interval = 0;
//LED Pins
int red=7;
int blue=6;
int green=5;

//Timer
SimpleTimer timer;


void callback()
{
  char buff1[2];
  char buff2[2];
  
  buff1[0] = id;
  buff1[1] = '\0';
  buff2[0] = command;
  buff2[1] = '\0';;
  //Serial.println(buff1);
  //Serial.println(buff2);
  XBee.write(buff1);
  if (command != 'N') XBee.write(buff2); 
  if (command == 'C') command = 'N';

  //Serial.println("******Sent*****");
}

void setup() {
  // put your setup code here, to run once:
  XBee.begin(9600);
  Serial.begin(9600);

  //LED Config
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(4, INPUT);
 
  digitalWrite(green,HIGH);
  digitalWrite(blue,HIGH);
  digitalWrite(red,HIGH);

  //timer
  timer.setInterval(2300, callback);
 
  infected = false;
  leader = true;
  Serial.println("Intializing...");
  //delay(2000);
}

void loop() 
{  
  timer.run();

  /*char buff1[2];
  char buff2[2];
  
  buff1[0] = id;
  buff1[1] = '\0';
  buff2[0] = command;
  buff2[1] = '\0';;
  //Serial.println(buff1);
  //Serial.println(buff2);
  XBee.write(buff1);
  if (command != 'N') XBee.write(buff2);  
  delay(1250);*/


  uint8_t msg = 0;
  
  while (XBee.available())
  {
    msg = XBee.read();
    //Serial.write(msg);
    if (msg == '1' || msg == '2' || msg == '3' || msg == '4' || msg == 'C' || msg == 'I') {
                
      Serial.print("MSG content: ");
      Serial.write(msg);
      Serial.println("");
      if(msg == '1')
      {
        id_list[1] = 1;
        id_time[1] = millis();
        Serial.println(id_list[1]);
        Serial.println(id_time[1]);
      }
      if(msg == '2')
      {
        id_list[2] = 2;
        id_time[2] = millis();
        Serial.println(id_list[2]);
        Serial.println(id_time[2]);
      }
      if(msg == '3')
      {
        id_list[3] = 3;
        id_time[3] = millis();
        Serial.println(id_list[3]);
        Serial.println(id_time[3]);
      }
    
      if(msg == '4')
      {
        id_list[4] = 4;
        id_time[4] = millis();
        Serial.println(id_list[4]);
        Serial.println(id_time[4]);
      }
      if (msg == 'C')
      {
        if (!leader) {
          infected = false;
          command = 'C';
          immune_interval = millis() + 3000;
        }
      }
      if (msg == 'I') {
        if (!leader && (immune_interval < millis())) {
          infected = true;
          command = 'I';
        }
      }
  //Serial.println();
    }
  }
  
  /*Leader Assignment*/
  if (id == '2')
  {
     if (((id_time[1] + 15000) > millis())) {
      if (infected) command = 'I';
      leader = false;
     }
     else {
       leader = true;
       command = 'N';
     }
  }

  else if (id == '3')
  {
    if (((id_time[1] + 15000) > millis()) || ((id_time[2] + 15000) > millis())) {
      if (infected) command = 'I';
      leader = false;
    }
    else {
      command = 'N';
      leader = true;
    }
  }

  else if (id == '4')
  { 
    if (((id_time[1] + 15000) > millis()) || ((id_time[2] + 15000) > millis()) || ((id_time[3] + 15000) > millis())) {
      if (infected) command = 'I';
      leader = false;
    }
    else {
      command = 'N';
      leader = true; 
    }
  }

  if(leader)
  {    
    if(digitalRead(4)) //switchPressed
    {
      command = 'C';
    }
    digitalWrite(blue,HIGH);
    digitalWrite(green,LOW);
    digitalWrite(red,LOW);
  }
  else if(!leader && !infected)
  {
    if(digitalRead(4) && (immune_interval < millis())) //switchPressed
    {
       infected = true;
       command = 'I';
       digitalWrite(blue,LOW);
       digitalWrite(green,LOW);
       digitalWrite(red,HIGH);
    }
    digitalWrite(blue,LOW);
    digitalWrite(green,HIGH);
    digitalWrite(red,LOW);
  }
  else if(!leader && infected)
  {
    digitalWrite(blue,LOW);
    digitalWrite(green,LOW);
    digitalWrite(red,HIGH);
  }
}





