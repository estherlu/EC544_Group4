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
var CARStatus = "0";

sp = new SerialPort.SerialPort(portName, portConfig);

app.get('/', function(req, res){
  res.sendfile('index.html');
});

app.use('/default', express.static(__dirname + '/default'));
app.use('/font', express.static(__dirname + '/font'));

app.get('/default.css',function(req, res){
  res.sendfile('default.css');
});

app.get('/font.css',function(req, res){
  res.sendfile('font.css');
});

io.on('connection', function(socket){
  console.log('client connected');
  io.emit('updated remoteCar', CARStatus);
  socket.on('disconnect', function(){
    console.log('client disconnected');
  });
  socket.on('remoteCar', function(msg){
    CARStatus = msg;
    sp.write(msg + "\n");
    console.log('message: ' + msg); // + msg
    io.emit('updated remoteCar', msg);
  });
});

http.listen(3000, function(){
  console.log('listening on *:3000');
});

sp.on("open", function () {
  console.log('open');
  sp.on('data', function(receivedStatus) {
      io.emit('updated remoteCar', receivedStatus);
      console.log('received: ' + receivedStatus);
      CARStatus = receivedStatus;
  });
});