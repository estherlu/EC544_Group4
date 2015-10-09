function getDateTime() {
  //get the time and date, will be added to
    var now     = new Date(); 
    var year    = now.getFullYear();
    var month   = now.getMonth()+1; 
    var day     = now.getDate();
    var hour    = now.getHours();
    var minute  = now.getMinutes();
    var second  = now.getSeconds(); 
    //correct the numbers to two digits
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
    var dateTime = month+ "/" + day + " " + hour+':'+minute+':'+second;   
     return dateTime;
}

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

sp = new SerialPort.SerialPort(portName, portConfig);

 app.use('/Data', express.static(__dirname + '/Data'));
  app.use('/images', express.static(__dirname + '/images'));
   app.use('/Style', express.static(__dirname + '/Style'));

app.get('/', function(req, res){
  res.sendfile('index.html');
});

app.get('/realtime', function(req, res){
  res.sendfile('index.html');
});

app.get('/style.css',function(req, res){
  res.sendfile('style.css');
});

app.get('/historical',function(req, res){
  res.sendfile('historical.html');
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
  //listen on localhost port 3000
  console.log('listening on *:3000');
});

sp.on("open", function () {
  console.log('open');
  sp.on('data', function(data){
      io.emit("chat message", data);
  });

//creat an array to save the latest temperature of each sendor
  var num = 4;
  var Temps = [];
  var tempObjs = [];
  var avgObj;
  for(var i = 0; i < num; i++){
    Temps[i] = 0;
  }
  var avg = 0;
  var fs = require('fs');
  var avgfile = '.\\Data\\Average.csv';
  var aimfile = [];
  for(var i = 1; i <= num; i++){
    aimfile[i] = '.\\Data\\Device' + i + '' + '.csv';
  }
  
  sp.on('data', function(data) {
    var dat = data.split(":");
    var index = parseInt(dat[0]);
    var timenow = getDateTime();

//save the temperature to the right position
    for(var j = 1; j <= num; j++){
      if(index == j) {
        Temps[j] = parseFloat(dat[1]);
        tempObjs[j] = {time: timenow, temp: dat[1]};
        fs.appendFile(aimfile[j], (timenow + ',' + dat[1] + '\n'), function(err){
          if (err) throw err;
          console.log('data was appended to file!');
        });
        //console.log(dat[0] + ' ' + JSON.stringify(tempObjs[j]));
        break;
      }
    }

//calculate the average
    var count = num;
    for(var k = 0; k < num; k++){
      avg = avg + Temps[k];
      if(Temps[k] == 0) count = count - 1;   
    }

    if(count > 0) avg = avg / count;
    var avg1 = avg.toPrecision(4) + '';//only save 5 digits after the decimal point
    avgObj = {temp: avg1, time: timenow};
    fs.appendFile(avgfile, (timenow+ ',' + avg1 + '\n'), function(err){
      if (err) throw err;
      console.log('avg was appended to file!');
    });
    console.log('data received: ' + dat[0] + ': ' + dat[1]);// print the received temperature in the console
    //io.emit("chat message", "Current room average temperature: " + avg1 + "°C at " + timenow);//add date and time to the message
    avg = 0;
  });
});

