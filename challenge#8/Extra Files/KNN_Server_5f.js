var SerialPort = require("serialport");
var app = require('express')();
var xbee_api = require('xbee-api');

var C = xbee_api.constants;
var XBeeAPI = new xbee_api.XBeeAPI({
  api_mode: 2
});

var portName = process.argv[2];

var sampleDelay = 3000;

var app = require('express')();
var express=require('express');
var http = require('http').Server(app);
var io = require('socket.io')(http);
var KNN = require('ml-knn');

app.use('/fonts', express.static(__dirname + 'webpage/fonts'));
app.use('/images', express.static(__dirname + '/webpage/images'));
app.use('/', express.static(__dirname + '/webpage'));


app.get('/localization', function(req, res){
  res.sendfile('webpage/index.html');
});

var knn = new KNN();

var trainingSet = [
/*Round 1*/
[35, 49, 74, 92],
[50, 59, 65, 87],
[56, 62, 69, 84],
[58, 65, 63, 93],
[62, 66, 53, 77],
[66, 72, 59, 75],
[63, 79, 36, 59],

[76, 70, 35, 71],

[71, 59, 35, 77],
[75, 54, 57, 91],
[74, 55, 56, 97],
[64, 55, 60, 100],
[63, 59, 56, 101],
[50, 44, 64, 102],
[39, 32, 71, 102],

[40, 34, 82, 101],

/*Round 2*/
[36, 35, 73, 101],
[60, 65, 80, 96],
[64, 66, 71, 95],
[68, 75, 70, 95],
[74, 82, 57, 84],
[63, 78, 51, 75],
[75, 76, 34, 69],

[72, 71, 33, 75],

[75, 56, 36, 75],
[76, 60, 46, 83],
[67, 51, 53, 94],
[68, 62, 58, 99],
[55, 58, 66, 102],
[59, 43, 71, 102],
[61, 34, 67, 102],

[35, 38, 80, 102],

/*Round 3*/
[35, 51, 66, 89],
[52, 56, 72, 92],
[59, 62, 67, 87],
[62, 65, 62, 87],
[63, 70, 64, 76],
[65, 75, 51, 77],
[58, 76, 49, 55],

[87, 81, 39, 61],

[75, 61, 39, 72],
[76, 55, 50, 87],
[76, 51, 56, 98],
[63, 51, 66, 101],
[64, 45, 56, 95],
[56, 43, 66, 101],
[37, 34, 76, 102],
[36, 40, 82, 99]
]
var predictions = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16];

knn.train(trainingSet, predictions);

knn.k = 3;

var dataset = [[0,0,0,0]];

//Note that with the XBeeAPI parser, the serialport's "data" event will not fire when messages are received!
portConfig = {
  baudRate: 9600,
  parser: XBeeAPI.rawParser()
};

io.on('connection', function(socket){
  console.log('a user connected');
  socket.on('disconnect', function(){
  });
});

http.listen(4001, function(){
  //listen on localhost port 4000
  console.log('listening on *:4001');
});

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
});

XBeeAPI.on("frame_object", function(frame) {
  if (frame.type == 144){
    console.log("Beacon ID: " + frame.data[1] + ", RSSI: " + (frame.data[0]));
    if(frame.data[1] == 1)
    {
      dataset[0][0] = frame.data[0];
      console.log(dataset[0][0]);
    }
    if(frame.data[1] == 2)
    {
      dataset[0][1] = frame.data[0];
      console.log(dataset[0][1]);
    }
    if(frame.data[1] == 3)
    {
      dataset[0][2] = frame.data[0];
      console.log(dataset[0][2]);
    }
    if(frame.data[1] == 4)
    {
      dataset[0][3] = frame.data[0];
      console.log(dataset[0][3]);
    }

     var store = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0];

    for(var i=0; i<23; i++)
    {
      store[i] = knn.predict(dataset);
    }

    var frequency = {};  // array of frequency.
    var max = 0;  // holds the max frequency.
    var result;   // holds the max frequency element.
    for(var v in store) {
            frequency[store[v]]=(frequency[store[v]] || 0)+1; // increment frequency.
            if(frequency[store[v]] > max) { // is this frequency > max so far ?
                    max = frequency[store[v]];  // update max.
                    result = store[v];          // update result.
            }
    }

    io.emit('location', result);
    console.log("reach here");
    console.log("answer: " + result);
  }
});
