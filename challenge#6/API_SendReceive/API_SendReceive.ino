#include <XBee.h>
#include <SoftwareSerial.h>


XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
// create reusable response objects for responses we expect to handle 
ZBRxResponse rx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();

uint8_t dbCommand[] = {'D','B'};
AtCommandRequest atRequest = AtCommandRequest(dbCommand);

ZBTxStatusResponse txStatus = ZBTxStatusResponse();
AtCommandResponse atResponse = AtCommandResponse();


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
int switchState = 0;
uint8_t id = '3';
int id_list[5];
unsigned long id_time[5];
int leaderID = 0;
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
}

void loop() 
{
      /*Send Message*/
      uint8_t payload[] = {'X', 'X', id};
      // Specify the address of the remote XBee (this is the SH + SL)
      XBeeAddress64 addr64 = XBeeAddress64(0x00000000, 0x0000FFFF);
      // Create a TX Request
      ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
      // Send your request
      xbee.send(zbTx);

      
      delay(300);

      /*Receive Data*/
      xbee.getResponse().getZBRxResponse(rx);
    
     Serial.println();
     for (int i= 0; i < rx.getDataLength(); i++){
        if (rx.getData()[i] == 'X' && rx.getData()[i+1] == 'X')
        {   
           for (int j= i; j < i+3; j++)   
           {
              uint8_t data = rx.getData()[j];
              if (data == '1')
              {
                delay(100);
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
           }
           break;
        }
      }
      Serial.println();

      if (((id_time[1] + 1000) > millis()) || ((id_time[2] + 1000) > millis())) {
        leader = false;
      }
      else leader = true;
      
  switchState=digitalRead(8);
  //(write code)if the ID is lowest Leader == 1

  if(leader)
  {
    digitalWrite(blue,HIGH);
    digitalWrite(green,LOW);
    digitalWrite(red,LOW);
  }
  else
  { 
    if(switchState==1)
    { digitalWrite(red, HIGH);
      digitalWrite(green,LOW);
    }
  
  }
    delay(300);
}
