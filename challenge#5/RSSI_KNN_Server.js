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

var trainingSet = [[56,56,54,49], 
[73,49,55,51], 
[79,46,37,41], 
[71,47,40,49], 
[74,49,50,35], 
[76,43,48,31], 
[71,41,55,40], 
[63,45,45,45], 
[73,45,45,37], 
[73,46,42,35], 
[69,41,42,34], 
[69,38,50,34], 
[69,45,38,48], 
[69,45,43,48], 
[72,50,42,51], 
[71,45,42,46], 
[73,38,48,51], 
[78,47,39,50], 
[67,43,42,44], 
[67,55,51,49], 
[74,43,42,45], 
[77,50,52,43], 
[75,43,37,46], 
[78,45,46,41], 
[70,45,44,50], 
[67,42,40,43], 
[72,48,45,42], 
[76,40,47,46], 
[78,53,45,40], 
[76,45,41,39], 
[71,40,45,47], 
[72,39,44,50], 
[70,38,48,41], 
[79,39,41,46], 
[75,42,48,48], 
[73,38,49,46], 
[73,39,50,40], 
[74,37,44,43], 
[75,39,48,45], 
[77,43,45,48], 
[77,37,39,48], 
[76,39,37,51], 
[75,41,44,47], 
[70,37,44,48], 
[74,45,48,50], 
[76,40,42,47], 
[72,39,35,40], 
[78,50,37,53], 
[76,31,54,69], 
[74,41,42,57], 
[76,42,48,52], 
[77,41,46,54], 
[77,45,33,49], 
[78,45,31,50]]
var predictions = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54]

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