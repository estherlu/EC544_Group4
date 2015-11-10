var SerialPort = require("serialport");
var app = require('express')();
var xbee_api = require('xbee-api');

var C = xbee_api.constants;
var XBeeAPI = new xbee_api.XBeeAPI({
  api_mode: 2
});

var math = require('mathjs');

var portName = process.argv[2];

var sampleDelay = 3000;

/*var beacon1_rssi = 0;
var beacon2_rssi = 0;
var beacon3_rssi = 0;
var beacon4_rssi = 0;
//var obj_x_coord = 0;
//var obj_y_coord = 0;*/

var d = [0,0,0,0];


//Note that with the XBeeAPI parser, the serialport's "data" event will not fire when messages are received!
portConfig = {
	baudRate: 9600,
  parser: XBeeAPI.rawParser()
};

var sp;
sp = new SerialPort.SerialPort(portName, portConfig);


//Create a packet to be sent to all other XBEE units on the PAN.
// The value of 'data' is meaningless, for now.
var RSSIRequestPacket = {
  type: C.FRAME_TYPE.ZIGBEE_TRANSMIT_REQUEST,
  destination64: "000000000000ffff",
  broadcastRadius: 0x01,
  options: 0x00,
  data: "test"
}

var requestRSSI = function(){
  sp.write(XBeeAPI.buildFrame(RSSIRequestPacket));
}

sp.on("open", function () {
  console.log('open');
  requestRSSI();
  setInterval(requestRSSI, sampleDelay);
  console.log("Requested RSSI");
});

N = 4; // Number of Beacons

var D = math.matrix(math.zeros([N, 2]));
var B = math.zeros(N, 3);
var b = math.zeros(N);

XBeeAPI.on("frame_object", function(frame) {
  if (frame.type == 144){
    console.log("Beacon ID: " + frame.data[1] + ", RSSI: " + (frame.data[0]));
    if (frame.data[1] == '1')
    { 
      d[0] = (d[0]+(parseInt(frame.data[0]) - 31) * 20)/2; //cm
      //math.subset(B, math.index(0, 0), 1); //B(0,0)
      B.subset(math.index(0, 0), 0); //x-coord of Beacon 0
      B.subset(math.index(0, 1), 0); //y
      B.subset(math.index(0, 2), d[0]); // RSSI
      //console.log(parseInt(frame.data[0]));
      //console.log(B.subset(math.index(0, 2)));
    }
    if (frame.data[1] == '2') 
    { 
      d[1] = (d[1]+(parseInt(frame.data[0]) - 31) * 20)/2
      B.subset(math.index(1, 0), 0); //x-coord of Beacon 1
      B.subset(math.index(1, 1), 400); //y
      B.subset(math.index(1, 2), d[1]); // RSSI

    }
    if (frame.data[1] == '3')
    {
      d[2] = (d[2]+(parseInt(frame.data[0]) - 31) * 20)/2
      B.subset(math.index(2, 0), 200); //x-coord of Beacon 2
      B.subset(math.index(2, 1), 0); //y
      B.subset(math.index(2, 2), d[2]); // RSSI

    }

     B.subset(math.index(2, 0), 400); //x-coord of Beacon 2
    B.subset(math.index(2, 1), 0); //y
     B.subset(math.index(2, 2), 100);

    if (frame.data[1] == '4')
    {
      d[3] = (d[3]+(parseInt(frame.data[0]) - 31) * 20)/2
      B.subset(math.index(3, 0), 400); //x-coord of Beacon 0
      B.subset(math.index(3, 1), 400); //y
      B.subset(math.index(3, 2), d[3]); // RSSI
    }
    B.subset(math.index(3, 0), 400); //x-coord of Beacon 0
      B.subset(math.index(3, 1), 400); //y
      //B.subset(math.index(3, 2), parseInt(frame.data[0])); // RSSI
      B.subset(math.index(3, 2), 100);
    //console.log(B);

    /*Apply calculations*/
    for (var i=0; i<3; i++)
  {
    var temp1 = B.subset(math.index(0, 0)) - B.subset(math.index(i+1, 0));
    var temp2 = B.subset(math.index(0, 1)) - B.subset(math.index(i+1, 1));
    var temp3 = math.square(B.subset(math.index(0, 0))) - math.square(B.subset(math.index(i+1, 0))) + math.square(B.subset(math.index(0, 1))) 
                - math.square(B.subset(math.index(i+1, 1))) - math.square(B.subset(math.index(0, 2))) + math.square(B.subset(math.index(i+1, 2)));
    D.subset(math.index(i, 0), temp1);
    D.subset(math.index(i, 1), temp2);

    b.subset(math.index(i), temp3); 
  }


  D = math.multiply(D, 2);
  //console.log("D " + D);

  var DT = math.transpose(D);

  var Q = math.multiply(math.inv(math.multiply(DT, D)), math.multiply(DT, b));

  //console.log("DT " + DT);
  console.log("Q " + Q);
  }
});
/*


for (var i=0; i<3; i++)
{
  var temp1 = math.subset(B, math.index(0, 0)) - math.subset(B, math.index(i+1, 0));
  var temp2 = math.subset(B, math.index(0, 1)) - math.subset(B, math.index(i+1, 1));
  var temp3 = math.square(math.subset(B, math.index(0, 0))) - math.square(math.subset(B, math.index(i+1, 0))) + math.square(math.subset(B, math.index(0, 1))) 
              - math.square(math.subset(B, math.index(i+1, 1))) - math.square(math.subset(B, math.index(0, 2))) + math.square(math.subset(B, math.index(i+1, 2)));
  math.subset(D, math.index(i, 0), temp1);
  math.subset(D, math.index(i, 1), temp2);

  math.subset(b, math.index(i), temp3); 
}

math.multiply(D, 2);

var DT = math.transpose(D);

//var Q = math.multiply(math.inv(math.multiply(DT, D)), math.multiply(DT, b));
 
*/
/*D = zeros(BeaconN -1, 2);
b = zeros(BeaconN -1, 1);
for i = 1 : BeaconN -1
    D(i, :) = [ B(1,1) - B(i+1, 1), B(1, 2) - B(i+1, 2) ];
    b(i) = B(1,1)^2 - B(i+1,1)^2 + B(1,2)^2 - B(i+1,2)^2 - B(1,3)^2 + B(i+1,3)^2;
end
D = 2 * D;
DT = D';
Q = (DT * D) \ DT * b;
x = Q(1);
y = Q(2);



N = 4;
D = [0,0,0,0,0,0,0,0];
b = [0,0,0,0];

for (int i = 1; i<=3; i++)
{
  D[2*i] =2* ;
  D[2*i + 1] = 2*;
  b[i];
}

b = zeros(BeaconN -1, 1);
for i = 1 : BeaconN -1
    D(i, :) = [ B(1,1) - B(i+1, 1), B(1, 2) - B(i+1, 2) ];
    b(i) = B(1,1)^2 - B(i+1,1)^2 + B(1,2)^2 - B(i+1,2)^2 - B(1,3)^2 + B(i+1,3)^2;
end
D = 2 * D;
DT = D';
Q = (DT * D) \ DT * b;
x = Q(1);
y = Q(2);*/