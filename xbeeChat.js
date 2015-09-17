function getDateTime() {
    var now     = new Date(); 
    var year    = now.getFullYear();
    var month   = now.getMonth()+1; 
    var day     = now.getDate();
    var hour    = now.getHours();
    var minute  = now.getMinutes();
    var second  = now.getSeconds(); 
    if(month.toString().length == 1) {
        var month = '0'+month;
    }
    if(day.toString().length == 1) {
        var day = '0'+day;
    }   
    if(hour.toString().length == 1) {
        var hour = '0'+hour;
    }
    if(minute.toString().length == 1) {
        var minute = '0'+minute;
    }
    if(second.toString().length == 1) {
        var second = '0'+second;
    }   
    var time = hour+':'+minute+':'+second;   
     return time;
}

var SerialPort = require("serialport");
var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);

var portName = process.argv[2],
portConfig = {
	baudRate: 9600,
	parser: SerialPort.parsers.readline("\n")
};

var sp;

sp = new SerialPort.SerialPort(portName, portConfig);

app.get('/', function(req, res){
  res.sendfile('index.html');
});

io.on('connection', function(socket){
  console.log('a user connected');
  socket.on('disconnect', function(){
  });
  socket.on('chat message', function(msg){
    io.emit('chat message', msg);
    sp.write(msg + "\n");
  });
});

http.listen(3000, function(){
  console.log('listening on *:3000');
});

sp.on("open", function () {
  console.log('open');


  var num = 4;
  var Temps = [];
  for(var i = 0; i < num; i++){
    Temps[i] = 0;
  }
  var avg = 0;
  
  sp.on('data', function(data) {
    var dat = data.split(":");
    var index = parseInt(dat[0]);

    for(var j = 0; j < num; j++){
      if(index == j) {Temps[j] = parseFloat(dat[1]); break;}
    }

    var count = num;
    for(var k = 0; k < num; k++){
      avg = avg + Temps[k];
      if(Temps[k] == 0) count = count - 1;   
    }

    if(count > 0) avg = avg / count;
    var avg1 = avg.toPrecision(5) + '';
 
    console.log('data received: ' + dat[0]);
    io.emit("chat message", "Current room average temperature: " + avg1 + "°C at " + getDateTime());
    avg = 0;
  });
});

