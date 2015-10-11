var SerialPort = require("serialport");
var app = require('express')();
var express=require('express');
var http = require('http').Server(app);
var io = require('socket.io')(http);

var portName = process.argv[2],
portConfig = {
  baudRate: 9600,
  parser: SerialPort.parsers.readline("\n")
};

var sp;
var LEDStatus = "0000";

sp = new SerialPort.SerialPort(portName, portConfig);

app.get('/', function(req, res){
  res.sendfile('index.html');
});

io.on('connection', function(socket){
  console.log('luminaire connected');
  socket.on('disconnect', function(){
    console.log('luminaire disconnected');
  });
  socket.on('bStates', function(msg){
    //io.emit('bStates', msg);
    LEDStatus = msg;
    sp.write(msg + "\n");
    console.log('message: ' + msg); // + msg
  });
});

http.listen(3001, function(){
  console.log('listening on *:3001');
});

sp.on("open", function () {
  console.log('open');
  //sp.on('data', function(data) {
  //  console.log('data received: ' + data);
  //  console.log('LED1 Status: ' + data[0]);
  //  console.log('LED2 Status: ' + data[1]);
  //  console.log('LED3 Status: ' + data[2]);
  //  console.log('LED4 Status: ' + data[3]);
  //  io.emit("status", "An XBee says: " + data);
    //LEDStatus = data;
  //});

  //sp.write(LEDStatus, function(err, results) {
  //    console.log('err ' + err);
  //    console.log('results ' + results);
  //});
});