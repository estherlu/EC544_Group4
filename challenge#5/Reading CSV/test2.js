var csv = require('csv');
var fs = require('fs');
var parse = require('csv-parse');

var parser = parse({delimiter: ';'}, function(err, data){
  console.log(data);
});

fs.createReadStream('my.csv').pipe(parser);