#include <XBee.h>
#include <SoftwareSerial.h>
#include <SimpleTimer.h>



XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
// create reusable response objects for responses we expect to handle 
ZBRxResponse rx = ZBRxResponse();

uint8_t dbCommand[] = {'D','B'};
AtCommandRequest atRequest = AtCommandRequest(dbCommand);

ZBTxStatusResponse txStatus = ZBTxStatusResponse();
AtCommandResponse atResponse = AtCommandResponse();


/*timer*/
SimpleTimer timer;

// Define NewSoftSerial TX/RX pins
// Connect Arduino pin 2 to Tx and 3 to Rx of the XBee
// I know this sounds backwards, but remember that output
// from the Arduino is input to the Xbee
#define ssRX 2
#define ssTX 3
SoftwareSerial nss(ssRX, ssTX);
SoftwareSerial xbeeSerial(2,3);

int red=7;
int blue=6;
int green=5;
bool infected = false;
uint8_t id = '3';
uint8_t commandL = '0';
uint8_t commandN = '0';
boolean first = false;
unsigned long immuneTime = 0;
boolean immune = false;
int id_list[5];
unsigned long id_time[5] = {0, 0, 0, 0 ,0};
bool leader = true;


void setup() {  
  // start serial
  Serial.begin(9600);
  xbeeSerial.begin(9600);
  xbee.setSerial(xbeeSerial);
  Serial.println("Initializing...");
  
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(8, INPUT);
  
  digitalWrite(green,HIGH);
  digitalWrite(blue,HIGH);
  digitalWrite(red,HIGH);

  //timer
  timer.setInterval(1000, callback);
  delay(2000);
}

void loop() 
{
     timer.run();
      /*Receive Data*/

     xbee.readPacket();
      
    // so the read above will set the available up to
    // work when you check it.
    if (xbee.getResponse().isAvailable()) 
    {
      // if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE){
      // got something
      Serial.println();
      xbee.getResponse().getZBRxResponse(rx);

      if (rx.getOption() && ZB_PACKET_ACKNOWLEDGED)
            // the sender got an ACK
          Serial.println("packet acknowledged");
      
     // Serial.println(xbee.getResponse().getApiId(), HEX);

     for (int i= 0; i < rx.getDataLength(); i++)
     {
        if (rx.getData()[i] == 'X' && rx.getData()[i+1] == 'X')
        {   
           for (int j= i; j < i+5; j++)   
           {
              uint8_t data = rx.getData()[j];
              Serial.write(rx.getData()[j]);
              Serial.println();
              if (data == '1')
              {
                id_list[1] = 1;
                id_time[1] = millis();
                //Serial.write(rx.getData()[j]);
                Serial.println("XXXXXXXX  1 XXXXXXXXX");
                Serial.write(id_list[1]);
                Serial.write(id_time[1]);
                //Serial.write(millis());
              }
              else if (data == '2')
              {
                id_list[2] = 2;
                id_time[2] = millis();
                //Serial.write(rx.getData()[j]);
                Serial.println("XXXXXXXX  2 XXXXXXXXX");
                Serial.print(id_list[2]);
                Serial.print(id_time[2]);
              }
              else if (data == '3')
              {
                id_list[3] = 3;
                id_time[3] = millis();
                Serial.println("XXXXXXXX  3 XXXXXXXXX");
                Serial.print(id_list[3]);
                Serial.print(id_time[3]);
              }
              else if (data == '4')
              {
                id_list[4] = 4;
                id_time[4] = millis();
                Serial.println("XXXXXXXX  4 XXXXXXXXX");
                Serial.print(id_list[4]);
                Serial.print(id_time[4]);
              }

              if (data == 'L' && rx.getData()[j] == 'C') //if receiving clear command from leader, uninfect
              {
                Serial.println("&^%^&*here");
                infected = false;
                immune = true;
                immuneTime = millis();
              }

              /*else if (rx.getData()[j] == 'C') //if receiving clear command from leader, uninfect
              {
                commandN = 'C';
                infected = false;
                immune = true;
              }*/

              //if ( immune && immuneTime + 10000 > millis())
              //{
                if (data == 'N' && rx.getData()[j+1] == 'I') //if non-leader node is infected
                  infected = true;
              //}
              //else 
                //immune = false;
           }
           
           break;
        }
        }
      }
  //  }


      if (id == '2')
      {
         if (((id_time[1] + 10000) > millis())) leader = false;
         else leader = true;
      }

      else if (id == '3')
      {
        if (((id_time[1] + 10000) > millis()) || ((id_time[2] + 10000) > millis())) leader = false;
        else leader = true;
      }

      else if (id == '4')
      { 
        if (((id_time[1] + 10000) > millis()) || ((id_time[2] + 10000) > millis()) || ((id_time[3] + 10000) > millis())) leader = false;
        else leader = true; 
      }
      
      
      //(write code)if the ID is lowest Leader == 1
    
      if(leader)
      {    
        if(digitalRead(8)) //switchPressed
          first = true;

        
        digitalWrite(blue,HIGH);
        digitalWrite(green,LOW);
        digitalWrite(red,LOW);
      }
      else //not leader
      { 
        if(digitalRead(8)) //switchPressed
        {
          infected = true;
          commandN = 'I'; //Infect
        }
       
        if(!infected) //not infected
        {
            //commandN = 'C';
            digitalWrite(blue,LOW);
            digitalWrite(green, HIGH);
            digitalWrite(red,LOW);
        }

        /*if (immune)
        {
          delay(3000);
          immune = false;
        }*/
        
        if(infected)
        { 
          commandN = 'I'; 
          digitalWrite(blue,LOW);
          digitalWrite(red, HIGH);
          digitalWrite(green,LOW);
        }
      }
    
}//end loop

void callback()
{

  uint8_t command, isLeader;

 
  /*Send Message*/
  if(leader)
  {
    if (first)
    {
      first = false;
      commandL = 'C'; 
      Serial.println("jyuhgtrfedw");
    }
    else
      commandL = '0'; 

    isLeader = 'L';
    command = commandL;    
  }
  else 
  {
      isLeader = 'N';
      command = commandN;
  }


  uint8_t payload[] = {'X', 'X', id, isLeader, command};
  //uint8_t payload[] = {'X', 'X', id};
  // Specify the address of the remote XBee (this is the SH + SL)
  //XBeeAddress64 addr64 = XBeeAddress64(0x00000000, 0x0000FFFF);
  // Create a TX Request
  ZBTxRequest zbTx = ZBTxRequest(ZB_BROADCAST_ADDRESS, payload, sizeof(payload));
  // Send your requests
  xbee.send(zbTx);

  delay(200);
  
  Serial.println("**** Sent");
}
