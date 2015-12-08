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
  res.sendfile('index.html');
});

var knn = new KNN();

var trainingSet = [
/*Round 1*/
[36, 76, 40, 100]
[59, 66, 62, 54, 94]
[58, 69, 51, 97]
[72, 62, 55, 99]
[75,61, 57, 80]
[56, 45, 60, 79]
[82, 36, 69, 73]

[71, 48, 78, 78]

[66, 36, 74, 68]
[57, 44, 67, 89]
[51, 58, 71, 90]
[58, 63, 69, 98]
[48, 71, 66, 96]
[51, 53, 55, 101]
[32, 69, 44, 97]

[39, 73, 34, 102]

/*Round 2*/
[36, 71, 35, 99]
[57, 67, 48, 100]
[61, 69, 49, 101]
[63, 68, 63, 81]
[69, 65, 58, 85]
[76, 59, 60, 66]
[67, 34, 56, 68]

[71, 37, 73, 64]

/*[79, 51, 76, 69]
[53, 34, 68, 84]
[52, 50, 75, 78]
[57, 51, 65, 88]
[50, 55, 64, 95]
[46, 56, 59, 100]*/

[52, 38, 71, 69]
[49, 48, 74, 84]
[57, 55, 75, 94]
[49, 50, 61, 89]
[42, 66, 51, 90]
[45, 57, 49, 102]
[34, 71, 38, 101]

[43, 75, 36, 101]

/*Round 3*/
[45, 72, 36, 91]
[57, 72, 49, 86]
[67, 70, 59, 88]
[70, 63, 51, 82]
[72, 66, 61, 87]
[67,52, 57, 80]
[64, 34, 61, 69]

[74, 44, 77, 65]

[61, 35, 77, 67]
[51, 48, 74, 78]
[57, 56, 70, 97]
[47, 53, 64, 98]
[48, 63, 61, 100]
[43, 57, 46, 97]
[34, 63, 34, 95]

[35, 66, 38, 99]




]
var predictions = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16]

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

http.listen(4000, function(){
  //listen on localhost port 4000
  console.log('listening on *:4000');
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

    var ans = knn.predict(dataset);
    io.emit('location', ans);
    console.log("answer: " + ans);
  }
});