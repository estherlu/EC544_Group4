#include <XBee.h>
#include <SoftwareSerial.h>
SoftwareSerial xbeeSerial(2,3);
// Create an XBee object at the top of your sketch
XBee xbee = XBee();
void setup(){
  Serial.begin(9600);
  xbeeSerial.begin(9600);
  xbee.setSerial(xbeeSerial);
}
void loop(){
// Create an array for holding the data you want to send.
    uint8_t payload[] = { '2', 'H', 'i' };

// Specify the address of the remote XBee (this is the SH + SL)
    XBeeAddress64 addr64 = XBeeAddress64(0x00000000, 0x0000ffff);

// Create a TX Request
    ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));

// Send your request
    xbee.send(zbTx);
}
