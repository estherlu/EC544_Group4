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

app.use('/Button', express.static(__dirname + '/Button'));
app.use('/Style', express.static(__dirname + '/Style'));

app.get('/style.css',function(req, res){
  res.sendfile('style.css');
});

app.get('/buttons.css',function(req, res){
  res.sendfile('buttons.css');
});

io.on('connection', function(socket){
  console.log('client connected');
  io.emit('updated bStates', LEDStatus);
  socket.on('disconnect', function(){
    console.log('client disconnected');
  });
  socket.on('bStates', function(msg){
    LEDStatus = msg;
    sp.write(msg + "\n");
    console.log('message: ' + msg); // + msg
    io.emit('updated bStates', msg);
  });
});

http.listen(3000, function(){
  console.log('listening on *:3000');
});

sp.on("open", function () {
  console.log('open');
  sp.on('data', function(receivedStatus) {
      io.emit('updated bStates', receivedStatus);
      console.log('received: ' + receivedStatus);
      LEDStatus = receivedStatus;
  });
});