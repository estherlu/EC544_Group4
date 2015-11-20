
 
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


uint8_t id = '1';
int id_list[4];
unsigned long id_time[4];
int leaderID = 0;
boolean leader = false;

void setup() {  
  // start serial
  Serial.begin(9600);
  xbeeSerial.begin(9600);
  xbee.setSerial(xbeeSerial);
  Serial.println("Initializing...");
}

void loop() 
{
      /*Send Message*/
      uint8_t payload[] = {'X', 'X', '4', '4'};
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
                id_list[0] = 1;
                id_time[0] = millis();
                //Serial.write(rx.getData()[j]);
                Serial.println("XXXXXXXX  1 XXXXXXXXX");
                Serial.write(id_list[0]);
                Serial.write(id_time[0]);
              }
              else if (data == '2')
              {
                id_list[1] = 2;
                id_time[1] = millis();
                //Serial.write(rx.getData()[j]);
                Serial.println("XXXXXXXX  2 XXXXXXXXX");
                Serial.print(id_list[1]);
                Serial.print(id_time[1]);
              }
              else if (data == '3')
              {
                id_list[2] = 3;
                id_time[2] = millis();
                Serial.println("XXXXXXXX  3 XXXXXXXXX");
                Serial.print(id_list[2]);
                Serial.print(id_time[2]);
              }
              else if (data == '4')
              {
                id_list[3] = 4;
                id_time[3] = millis();
                Serial.println("XXXXXXXX  4 XXXXXXXXX");
                Serial.print(id_list[3]);
                Serial.print(id_time[3]);
              }
           }
           break;
        }
      }
      Serial.println();
 
    delay(300);
}
